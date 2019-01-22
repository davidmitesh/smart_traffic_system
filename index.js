//Importing libraries,models and dependencies
const express=require('express');
var util = require("util");
var fs = require('fs');
var mongoose= require('./server/db/mongoose.js');
var {location}= require('./server/models/trafficdensity.js');
var {driver}= require('./server/models/ambulance_drivers.js');
var bodyParser= require('body-parser')
var spawn = require("child_process").spawn;
var process = spawn('python',["imagedetection.py"]);



//important parameters in order for code to run
let app=express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended:true}));

//route to custom give the place code and number of people and no. of vehicles
app.post('/',(req,res)=>{
    data=new location({place_code:req.body.place_code,number_people:req.body.number});
    data.save().then((res)=>{
        console.log("data saved");
    });
});

//route to get the traffic data of the particular place by giving the place code
app.get('/trafficdata',(req,res)=>{
    location.find({place_code:req.query.place_code},(err,doc)=>{

        res.send(doc);
    });
    });

//  just for check purpose
app.get('/home',(req,res)=>{

            res.send("hey");
        });
//for ambulance related routes//-----------------------------------------

//for creating a new driver account
app.post('/dri_create',(req,res)=>{
    data=new driver({name:req.body.name,ambulance_no:req.body.ambulance_no});
    data.save().then((data)=>{
        res.send('new driver added to ambulance list');
    });
});

//for getting the status of the ambulance (includes the request for the
//particular traffic light of particualar place code.)
app.get('/post_request',(req,res)=>{
    driver.findOneAndUpdate({ambulance_no:req.body.ambulance_no},{place_code:req.body.place_code,status:req.body.status},(err,data)=>{
        res.send('successfully request submitted');
        //arduino interrupt code to signal the lane based on the place_code
        //location.
    });
});




// to send the data obtained from computer vision to the database every 4 seconds.
function intervalFunc() {
    fs.readFile('guru99.txt',function(err,data){
        location.findOneAndUpdate({place_code:2},{number_people:data},(err,result)=>{
            console.log("process completed");
        });
    });
}
setInterval(intervalFunc, 4000);


//making server to listen on port 3000
app.listen(3000,()=>{
    console.log('server is up');
});
