//express
var express = require('express')
var app = express()
var bodyParser = require('body-parser')
app.use(express.json())
app.use(bodyParser.json())
app.post('/sensor', function (request, response) {
	console.log(request.body)
	var timestamp = Date.now()
	var ref= db.ref(`sensor/${request.body.tempat}/${timestamp}/`)
	response.send(request.body)
	ref.update(
		request.body
	)
});
app.get('/',function(req,res)
{
	res.send('hello world');
})
app.listen(process.env.PORT||3000)