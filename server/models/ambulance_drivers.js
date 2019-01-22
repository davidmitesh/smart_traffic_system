const mongoose=require('mongoose');
const _=require('lodash');

var driver=new mongoose.Schema({

    name:{
        type:String
    },
    ambulance_no:{
        type:Number,
        default:0
    },
    status:{
        type:Boolean,
        default:false
    },
    place_code:{
        type:Number,
        default:0
    }
});
var driver=mongoose.model('driver',driver);
module.exports={driver};
