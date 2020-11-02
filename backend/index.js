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

app.post('/scoring', (req, res) => {
	const client = new Client({
        user: 'gydms',
        host: 'localhost',
        database: 'userdb',
        password: '1234',
        port: 5000,
    })
	client.connect();
	
	var filename = "test";
	var p_idx = 2;
	var testcase = '';

	console.log(req.body);
	console.log(unescape(req.body.srccode.toString()));

	switch(req.body.lang) {
	case "C":
		filename += ".c";
		break;
	case "JAVA":
		filename += ".c";
		break;
	case "PYTHON":
		filename += ".py";
		break;
	default:
		console.log("Error" + req.body);
		res.send("error");
	}
	
	
	client.query('SELECT idx,input,output FROM testcase WHERE p_idx ='+ p_idx, (err, psql_response) => {
		var str ='';
		str += '\"';
		for(item of psql_response.rows){
			str += item['input'] + ':' + item['output'] +':';
			
		}
		testcase = str.replace(/.$/,'\"');
		
		fs.writeFile(filename, unescape(req.body.srccode), function(err) {
		if (err == null) {
			const stdout = execSync('../marker/build/marker C ../../tes.c 1:536870912 '+ testcase);
			const data = JSON.stringify({"stdout":stdout});
			var temp = `${stdout}`;
			console.log(JSON.parse(temp).marking);
			//console.log(temp.split(','));
			//res.send();
		} else {
			console.log("fail");
		}
	})
	})
	
	
})

app.listen(PORT, () => {
	console.log(`API Server listening on port ${PORT}!`);
});

