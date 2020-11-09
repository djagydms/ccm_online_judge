const express = require('express');
var bodyParser = require('body-parser');
const fs = require('fs');
const { Client } = require('pg');
const cors = require('cors');
const execSync = require('child_process').execSync;

const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.post('/mkproblem', (req, res) => {

});

app.post('/ranking', (req, res) => {

});

var cnt = 0;

app.post('/scoring', (req, res) => {
	const client = new Client({
        user: 'gydms',
        host: 'localhost',
        database: 'userdb',
        password: '1234',
        port: 5000,
    })
	client.connect();
	
	/* TODO: proj id directory / user id file */
	var filename = "test" + cnt;
	var p_idx = 2;
	var testcase = '';

	cnt += 1;

	console.log(req.body);
	
	client.query('SELECT idx,input,output FROM testcase WHERE p_idx ='+ p_idx, (err, psql_response) => {
		var str ='';
		str += '\"';
		for(item of psql_response.rows){
			str += item['input'] + ':' + item['output'] +':';
			// TODO: passing the delimeter to marker (ex, : in testcase)
		}
		testcase = str.replace(/.$/,'\"');
		
		fs.writeFile(filename, unescape(req.body.srccode), function(err) {
			if (err == null) {
				const stdout = execSync('../marker/build/marker ' 
									+ req.body.lang + ' ' 
									+ filename 
									+ ' 1:536870912 '
									+ testcase); // TODO: limit compile, run time
				var temp = `${stdout}`;
				var json = JSON.parse(temp);
				console.log(temp);
				//console.log(JSON.parse(temp).marking); // marking, time (TODO: compile runtime)
				//console.log(temp.split(','));
				res.json(json);
			} else {
				console.log("fail");
			}
		});
	});
});

const PORT = 4000;

app.listen(PORT, () => {
	console.log(`API Server listening on port ${PORT}!`);
});

