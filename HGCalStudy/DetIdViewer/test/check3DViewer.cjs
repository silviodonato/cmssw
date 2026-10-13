// Smoke test for the 3D viewer: decodes the embedded data and exercises the selection with stub DOM/WebGL.
// Usage: node check3DViewer.cjs hgcal-d128-3d.html
const fs=require('fs'),vm=require('vm');
const html=fs.readFileSync(process.argv[2],'utf8');const script=html.match(/<script>([\s\S]*)<\/script>/)[1];
const el={};const mk=id=>{const e={id,value:'',textContent:'',checked:false,disabled:false,title:'',style:{},children:[],append(o){this.children.push(o)},
 querySelector(q){return q==='.stat'?mk(id+'stat'):Object.assign(mk(id+q),{clientWidth:800,clientHeight:600,getContext:t=>t==='2d'?new Proxy({},{get:()=>()=>{}}):gl,getBoundingClientRect:()=>({left:0,top:0}),addEventListener(){},setPointerCapture(){}})}};return el[id]=e};
const gl=new Proxy({getShaderParameter:()=>true,getProgramParameter:()=>true},{get:(o,k)=>k in o?o[k]:(typeof k==='string'&&k===k.toUpperCase()?1:()=>({}))});
const sb={document:{getElementById:id=>el[id]||mk(id),createElement:()=>mk('opt')},window:{devicePixelRatio:1},Blob,Response,DecompressionStream,Uint8Array,Float32Array,Uint32Array,Uint16Array,Int32Array,atob,console,Math,Object,Number,Array,JSON,Error,
 ResizeObserver:class{observe(){}},requestAnimationFrame:f=>setImmediate(f),setTimeout,clearTimeout};
for(const id of ['useAdc','useTot','sizeByQ','link'])mk(id).checked=true;mk('psize').value='0.8';mk('adcLSB').value='0.09765625';mk('totLSB').value='2.44140625';
(async()=>{await vm.runInNewContext(script,sb);await new Promise(r=>setTimeout(r,300));
 console.log('error:',JSON.stringify(el.error?.textContent));console.log(el.metadata.textContent);console.log('count:',el.count.textContent);
 console.log('stats:',el.pzstat?.textContent,'|',el.mzstat?.textContent);console.log('q range:',el.qlo.textContent,el.qhi.textContent);
 console.log('defaults z',el.zmin.value,el.zmax.value,'r',el.rmin.value,el.rmax.value,'adc',el.adcmin.value,el.adcmax.value,'tot',el.totmin.value,el.totmax.value,'toa',el.toamin.value,el.toamax.value);
 el.zmax.value='340';el.adcmin.value='100';el.useTot.checked=false;el.sample.onchange();console.log('filtered:',el.count.textContent);
})().catch(e=>{console.error(e);process.exitCode=1});
