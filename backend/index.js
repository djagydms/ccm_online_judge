const express = require('express');
var bodyParser = require('body-parser');
const fs = require('fs');
const { Client } = require('pg');
const cors = require('cors');
const { networkInterfaces } = require('os');
const execSync = require('child_process').execSync;
const Queue = require('queue');

const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));


const client = new Client({
	user: 'gydms',
	host: 'localhost',
	database: 'userdb',
	password: '1234',
	port: 5000,
})
client.connect();

app.post('/mkproblem', (req, res) => {
	console.log(req.body);
	var moment = require('moment');
	const sql = "INSERT INTO problem (p_idx,userid,explanation,compilelimit,runlimit,created_on)VALUES($1, $2, $3, $4, $5, $6) RETURNING *";
	const values = [req.body.number, req.body.userID, req.body.explanation, req.body.c_time, req.body.r_time, `${moment().format("YYYY-MM-DD HH:mm")}`]


	client.query(sql,values,(err, psql_response)=>{
		if (err) {
			console.log(err.stack)
		} else {
			console.log(psql_response.rows[0])
			const sql = "INSERT INTO testcase (p_idx,input,output)VALUES($1, $2, $3) RETURNING *";

			for(var i = 1; i <=req.body.caseNum; i++){
				const values = [req.body.number, req.body["input"+i],req.body["output"+i]]
				client.query(sql,values,(err, psql_response)=>{
					if (err) {
						console.log(err.stack)
					} else {
						console.log(psql_response.rows[0])
					}
				});
			}

			res.send("");
		}
	});
});

app.post('/getproblem', (req, res) => {
	console.log(req.body);

	client.query('SELECT explanation FROM problem WHERE p_idx ='+ req.body.p_number, (err, psql_response) => {
		
		if (err) {
			console.log(err.stack)
		} else {
			
			var ret = JSON.parse('{ "ret" :"' + psql_response.rows[0]['explanation'] +'" }');
			console.log(ret);
			res.json(ret);
		}
	});
});

app.post('/ranking', (req, res) => {
	console.log(req.body.lang);
	
	client.query("SELECT userid, runtime FROM userlog WHERE lang ='"+req.body.lang+"' AND p_idx ="+ req.body.p_number+" AND testcase NOT LIKE '%x%' ORDER BY runtime", (err, psql_response) => {
		
		var temp ='{"rank": [' ;
		
		if (err) {
			console.log(err.stack)
		} else {
			psql_response.rows.forEach(function(element) {
				temp += '{"id":"'+element.userid+'", "time":"'+element.runtime+'"},'
			});
			temp = temp.replace(/.$/,'');
			temp +=']}'
			var ret = JSON.parse(temp);
			console.log(ret);
			res.json(ret);
		}
	});
});

var queue = Queue();

var cnt = 0;

app.post('/scoring', (req, res) => {
	
	
	/* TODO: proj id directory / user id file */
	var filename = "/tmp/marking" + cnt;
	var testcase = '';

	cnt += 1;

	console.log(req.body);

	
	client.query('SELECT idx,input,output FROM testcase WHERE p_idx ='+ req.body.number, (err, psql_response) => {
		if(err){
			console.log(err.stack)
		}else{
			var str ='';
			str += '\"';
			for(item of psql_response.rows){
				str += unescape(item['input']) + ':' + unescape(item['output']) +':';
				// TODO: passing the delimeter to marker (ex, : in testcase)
			}
			testcase = str.replace(/.$/,'\"');

			client.query('SELECT compilelimit, runlimit FROM problem WHERE p_idx ='+ req.body.number, (err, psql_response) => {
				if (err) {
					console.log(err.stack)
				} else {
					var climit = psql_response.rows[0]['compilelimit'];
					var rlimit =  psql_response.rows[0]['runlimit'];
					
					fs.writeFile(filename, unescape(req.body.srccode), function(err) {
						if (err == null) {
							var container = queue.pop();
							var stdout;
							try {
									stdout = execSync('../marker/build/marker '
												+ req.body.lang + ' ' 
												+ filename + ' '
												+ ": "
												+ container + ':'
												+ '1:512 '
												+ testcase + ' '
												+ climit + ' '
												+ rlimit );
									queue.push(container);
									var temp = `${stdout}`;
									var json = JSON.parse(temp);
									console.log(temp);
									var moment = require('moment');
									const sql = "INSERT INTO userlog (p_idx,lang,userid,testcase,compiletime,runtime,created_on)VALUES($1, $2, $3, $4, $5, $6, $7) RETURNING *";
									const values = [req.body.number, req.body.lang ,req.body.userID,json.marking,null,json.exectime,`${moment().format("YYYY-MM-DD HH:mm")}`]						

									client.query(sql,values,(err, psql_response)=>{
									if (err) {
											console.log(err.stack)
									} else {
											console.log(psql_response.rows[0])
									}
									});
									res.json(json);
							} catch (error) {
									res.json("{\"status\":"+error+"}");
							}
					} else {
							console.log("fail");
						}
					});
				}
			});
		}
		
	});
});

const PORT = 4000;

app.listen(PORT, () => {
	console.log(`API Server listening on port ${PORT}!`);
	queue.push("build1", "build2", "build3");
});

