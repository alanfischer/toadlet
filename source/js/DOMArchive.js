mergeInto(LibraryManager.library, {
	DOMArchive__postset: 
               'Module["DOMArchive"] = _DOMArchive;',

	DOMArchive:function(engine){
		var toadlet=Module;
		
		return toadlet['Archive']['implement']({
			engine : engine,
			
			'openStream' : function(name,request){
				var toadlet=Module;
				var xmlHttp=new XMLHttpRequest();
				xmlHttp.open("GET",name);
				xmlHttp.onreadystatechange=function(){
					if(xmlHttp.readyState==4){
						if(xmlHttp.status==200){
							request['stringStreamReady'](xmlHttp.responseText);
						}
						else{
							request['streamException'](new Error());
						}
						request['delete']();
					}
				}.bind(this);
				xmlHttp.ontimeout=function(){
					request['streamException'](new Error());
					request['delete']();
				}.bind(this);
				xmlHttp.send();
				return true;
			},
			
			'openResource' : function(name,request){
				var toadlet=Module;
				var image=new Image();
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
						requext.resourceException(new Error());
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
					request['resourceException'](new Error());
					request['delete']();
				}.bind(this);
				image.src=name;
				return true;
			}
		});
	}
});
