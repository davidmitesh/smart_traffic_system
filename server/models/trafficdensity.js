
// a model to store the data of traffic including the no of people and
// no of vehicles in the particular place of certain place code.

const mongoose=require('mongoose');
const _=require('lodash');

var location=new mongoose.Schema({

    place_code:{
        type:Number,
        default:0
    },
    number_people:{
        type:Number,
        default:0
    }
});

var location=mongoose.model('location',location);
module.exports={location};
