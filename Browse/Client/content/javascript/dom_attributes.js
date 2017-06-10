//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

ConsolePrint("Starting to import dom_attributes.js ...");


window.attrStrToInt = new Map();

// TODO: Call in CEF Renderer OnContextInitialized
function AddDOMAttribute(attrStr, attrInt)
{
    if(typeof(attrStr) !== "string" || typeof(attrInt) !== "number")
    {
        console.log("Error: Invalid datatypes given when callin AddDOMAttribute.\nattrStr: "+attrStr+"\nattrInt: "+attrInt);
        return;
    }

    window.attrStrToInt.set(attrStr, attrInt);
}

function GetAttributeCode(attrStr)
{
    var attrInt = window.attrStrToInt.get(attrStr);
    if(attrInt === undefined)
        console.log("Error in GetAttributeCode: Tried to handle an attribute called '"+attrStr+"', but is unknown.");
    return attrInt;
}


function SendAttributeChangesToCEF(attrStr, domObj)
{
    // DEBUG
    // console.log("SendAttributeChangesToCEF called!");

    // console.log("typeof(domObj.getType): "+typeof(domObj.getType));
    if (typeof(domObj.getType) !== "function")
        return "Invalid 'getType' function!";
    
    // console.log("Fetching attrCode for attrStr: "+attrStr);
    var attrCode = GetAttributeCode(attrStr);
    // console.log("attrCode: "+attrCode);
    if(attrCode === undefined)
        return "Invalid attrCode: "+attrCode;

    var encodedData = FetchAndEncodeAttribute(domObj, attrStr);
    if (encodedData === undefined)
    {
        console.log("Error in SendAttributeChangesToCEF: Failed to encode attribute "+attrStr+"!");
        return "Invalid encoded data: "+encodedData;
    }
    
    var msg = "DOM#upd#"+domObj.getType()+"#"+domObj.getId()+"#"+attrCode+"#"+encodedData+"#";

    ConsolePrint(msg);
    // console.log("AttrChanges: "+msg);
    return msg;
}


// TODO: Automatically decide which kind of encoding will take place? (typeof(true) === "boolean")
window.attrStrToEncodingFunc = new Map();
var simpleReturn = (data) => { return data; };
var listReturn = (data) => { return data.reduce( (i,j) => {return i+";"+j; })};

window.attrStrToEncodingFunc.set("Rects", (data) => {
    // Add ';' between every element in each list, afterwards add ';' between each list
    // FUTURE TODO: Use another kind of delimiter in between list elements as between lists
        return data.map( (l) => { return l.reduce((n,m) => {return n+";"+m; }) }).reduce( (i,j) => {return i+";"+j; });
    }
);
window.attrStrToEncodingFunc.set("FixedId", simpleReturn);
window.attrStrToEncodingFunc.set("OverflowId", simpleReturn);
window.attrStrToEncodingFunc.set("Text", simpleReturn);
window.attrStrToEncodingFunc.set("IsPassword", (data) => {return data + 0; });  // implicity cast bool to number
window.attrStrToEncodingFunc.set("Url", simpleReturn);
window.attrStrToEncodingFunc.set("Options", listReturn);
window.attrStrToEncodingFunc.set("MaxScrolling", listReturn);
window.attrStrToEncodingFunc.set("CurrentScrolling", listReturn);

function FetchAndEncodeAttribute(domObj, attrStr)
{
    var encodeFunc = window.attrStrToEncodingFunc.get(attrStr);
    if(encodeFunc === undefined)
    {
        console.log("Error in FetchAndEncodeAttribute: There does not exist any encoding function for attribute '"+attrStr+"'!");
        return undefined;
    }

    // Definition: Each getter should be called getAttrStr for simplicity
    if(typeof(domObj["get"+attrStr]) !== "function")
    {
        console.log("Error in FetchAndEncodeAttribute: Could not find function 'get"+attrStr+"' in given DOM object!");
        return undefined;     
    }

    // Call attribute getter
    var data = domObj["get"+attrStr]();

    // Enode and return data
    return encodeFunc(data);
}



ConsolePrint("Successfully imported dom_attributes.js!");