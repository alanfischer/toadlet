mergeInto(LibraryManager.library, {
	$DOMArchive__postset: 'Module["DOMArchive"] = _DOMArchive;',
	$DOMArchive: {},

	DOMArchive__deps: ['$DOMArchive'],
	DOMArchive: function(){return Module.Archive.extend('DOMArchive',{
		__construct : function(engine) {
			this.__parent.__construct.call(this);
			this.engine=engine;
		},

		__destruct : function() {
			this.__parent.__destruct.call(this);
		},
		
		openStream : function(name,request){
			var toadlet=Module;
			
			request=request['clone']();
			
			var xmlHttp=new XMLHttpRequest();
			xmlHttp.overrideMimeType('text\/plain; charset=x-user-defined');
			xmlHttp.open("GET",name);
			xmlHttp.onreadystatechange=function(){
				if(xmlHttp.readyState==4){
					if(xmlHttp.status==200){
						request['stringStreamReady'](xmlHttp.responseText);
					}
					else{
						request['streamException'](new toadlet['Exception']("status=="+xmlHttp.status));
					}
					request['delete']();
				}
			}.bind(this);
			xmlHttp.ontimeout=function(){
				request['streamException'](new toadlet['Exception']("ontimeout"));
				request['delete']();
			}.bind(this);
			xmlHttp.send();
			
			return true;
		},
		
		openResource : function(name,request){
			var toadlet=Module;

			request=request['clone']();

			var image=new Image();
			image.crossOrigin = toadlet.crossOrigin || "anonymous";
			image.onload=function(){
				// TODO: Can we cleanly get the texture handle from emscripten?
				var gl=toadlet.ctx;
				var target,handle;
				
				var textureManager=this.engine['getTextureManager']();
				var textureFormat=new toadlet['TextureFormat'](toadlet['Dimension']['Dimension_D2'].value,toadlet['Format']['Format_RGBA_8'].value,image.naturalWidth,image.naturalHeight,1,0);

				var oldBindTexture=gl.bindTexture.bind(gl);
				gl.bindTexture=function(gltarget,glhandle){ target=gltarget; handle=glhandle; oldBindTexture(gltarget,glhandle); };
				var texture=textureManager['createTexture'](textureFormat);
				gl.bindTexture=oldBindTexture;

				if(texture==null){
					request['resourceException'](new toadlet['Exception']("texture==null"));
				}
				else{
					gl.bindTexture(target,handle);
					gl.texImage2D(target,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,image);
					gl.generateMipmap(target);
					gl.bindTexture(target,null);
					textureManager['manage'](texture,name);
					request['resourceReady'](texture);
					texture['delete']();
				}
				textureFormat['delete']();
				request['delete']();
			}.bind(this);
			image.onerror=function(){
				request['resourceException'](new toadlet['Exception']("onerror"));
				request['delete']();
			}.bind(this);
			image.src=name;

			return true;
		}
	})}
});
