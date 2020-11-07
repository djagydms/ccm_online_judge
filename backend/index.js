const express = require('express');
var bodyParser = require('body-parser');
const fs = require('fs');
const { Client } = require('pg');

const execSync = require('child_process').execSync;

const app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(function (req, res, next) {

    // Website you wish to allow to connect
    res.setHeader('Access-Control-Allow-Origin', '*');

    // Request methods you wish to allow
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');

    // Request headers you wish to allow
    res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');

    // Set to true if you need the website to include cookies in the requests sent
    // to the API (e.g. in case you use sessions)
    res.setHeader('Access-Control-Allow-Credentials', false);

    // Pass to next layer of middleware
    next();
});
const PORT = 4000;

app.post('')

app.post('/mkproblem', (req, res) => {

});

app.post('/ranking', (req, res) => {

});

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
	var filename = "test";
	var p_idx = 2;
	var testcase = '';

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
				const data = JSON.stringify({"stdout":stdout});
				var temp = `${stdout}`;
				console.log(testcase);
				console.log(temp);
				//console.log(JSON.parse(temp).marking); // marking, time (TODO: compile runtime)
				//console.log(temp.split(','));
				res.send(temp);
			} else {
				console.log("fail");
			}
		})
	})
	
	
})

app.listen(PORT, () => {
	console.log(`API Server listening on port ${PORT}!`);
});

