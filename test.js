const express=require('express');
let app=express();


app.get('/home',(req,res)=>{
    place_code=Math.random()*5;
    number_people=Math.random()*5;
    var obj=[{place_code,number_people}]
    res.send(obj);
});

app.listen(3000,()=>{
    console.log('server is up');
});
