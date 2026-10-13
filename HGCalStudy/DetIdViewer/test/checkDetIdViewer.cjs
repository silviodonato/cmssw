// Exercise the standalone viewer's controls with a minimal DOM/canvas test double.
const fs=require('fs'),vm=require('vm'),assert=require('assert');
const elements={};
const context=new Proxy({}, {get:(o,k)=>o[k]||(()=>{}),set:(o,k,v)=>(o[k]=v,true)});
function element(id='') {const e={id,value:'',textContent:'',disabled:true,children:[],width:0,height:0,
 append(o){this.children.push(o);if(this.children.length===1)this.value=String(o.value)},replaceChildren(){this.children=[];this.value=''},
 getBoundingClientRect(){return {width:1000,height:652,left:0,top:0}},getContext(){return context},addEventListener(n,f){this['on'+n]=f},setPointerCapture(){},click(){this.onclick?.()},};return e;}
for(const id of ['map','metadata','sample','scaleLabel','layer','side','wafer','find','reset','zin','zout','status','detail','error','search'])elements[id]=element(id);
let script=fs.readFileSync(process.argv[2]||'hgcal-d128-all-detids.html','utf8').match(/<script>([\s\S]*)<\/script>/)[1];
const sandbox={document:{getElementById:id=>elements[id],createElement:()=>element()},window:{devicePixelRatio:1},Blob,Response,DecompressionStream,Uint8Array,atob,console,
 Path2D:class{moveTo(){}lineTo(){}closePath(){}},ResizeObserver:class{observe(){}},requestAnimationFrame:f=>setImmediate(f)};
(async()=>{await vm.runInNewContext(script,sandbox);await new Promise(r=>setImmediate(r));
 assert.equal(elements.error.textContent,'');assert.equal(elements.layer.children.length,26);assert.equal(elements.side.children.length,2);
 assert(elements.metadata.textContent.includes('392,038'));assert(elements.status.textContent.includes('10,136'));
 elements.layer.value='26';elements.layer.onchange();assert(elements.status.textContent.includes('Layer 26'));
 elements.side.value='-1';elements.side.onchange();assert(elements.status.textContent.includes('−z'));
 elements.search.value='0x8010080a';elements.find.click();assert(elements.detail.textContent.includes('2148534282'));assert.equal(elements.layer.value,1);assert.equal(elements.side.value,1);
 elements.search.value='0';elements.find.click();assert(elements.error.textContent.includes('not found'));
 assert.equal(elements.sample.value,'2');assert(elements.detail.textContent.includes('ADC')||elements.detail.textContent.includes('TOT'));
 elements.sample.value='0';elements.sample.onchange();assert(elements.scaleLabel.textContent.includes('sample 0'));
 elements.sample.value='2';elements.sample.onchange();
 elements.zin.click();elements.zout.click();elements.reset.click();
 console.log('PASS: embedded data decode; 26 layers; both endcaps; slice selection; hexadecimal DetId lookup; invalid lookup; zoom and fit handlers');
})().catch(e=>{console.error(e);process.exitCode=1});
