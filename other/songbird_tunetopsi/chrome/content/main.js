// Make a namespace.
if (typeof Tunetopsi == 'undefined') {
  var Tunetopsi = {};
}
/**
 * UI controller that is loaded into the main player window
 */
Tunetopsi.Controller = {
  /**
   * Called when the window finishes loading
   */
  onLoad: function() {

    // initialization code
    this._initialized = true;
    this._strings = document.getElementById("tunetopsi-strings");
    const Ci = Components.interfaces;
    Components.utils.import("resource://app/jsmodules/sbProperties.jsm");
    var gMM = Components.classes["@songbirdnest.com/Songbird/Mediacore/Manager;1"].getService(Components.interfaces.sbIMediacoreManager);
    var userEnvironment = Components.classes["@mozilla.org/process/environment;1"].getService(Components.interfaces.nsIEnvironment);
    var Utf8 = {
    // public method for url encoding
        encode : function (string) {
            string = string.replace(/\r\n/g,"\n");
            var utftext = "";
            for (var n = 0; n < string.length; n++) {
                var c = string.charCodeAt(n);
                if (c < 128) {
                    utftext += String.fromCharCode(c);
                }else if((c > 127) && (c < 2048)) {
                    utftext += String.fromCharCode((c >> 6) | 192);
                    utftext += String.fromCharCode((c & 63) | 128);
                }else {
                    utftext += String.fromCharCode((c >> 12) | 224);
                    utftext += String.fromCharCode(((c >> 6) & 63) | 128);
                    utftext += String.fromCharCode((c & 63) | 128);
                }
            }
            return utftext;
        },
    }
    var systype = {
        get : function(){
            var prov = Components.classes["@mozilla.org/file/directory_service;1"];
            prov = prov.getService(Ci.nsIDirectoryServiceProvider);
            var result = {};
            var sysDrive = prov.getFile("DrvD", result);
            if(sysDrive != null){
                return sysDrive.path;
            }
            return null;
        },
    }
    var path = "";
    if(systype.get()=="C:"){
        var userProfile = userEnvironment.get("LOCALAPPDATA");
        path = userProfile+"\\Psi+\\tune";
    }else if(systype.get()=="/"){
        var userProfile = userEnvironment.get("HOME");
        path = userProfile+"/.cache/Psi+/tune";
    }
    var tuneFile = {
        write : function (data) {
            var file = Components.classes["@mozilla.org/file/local;1"].createInstance(Components.interfaces.nsILocalFile);
            file.QueryInterface(Components.interfaces.nsIFile);
            if(path != "" || (path != null)){
                file.initWithPath(path);
                var foStream = Components.classes["@mozilla.org/network/file-output-stream;1"].createInstance(Components.interfaces.nsIFileOutputStream);
                foStream.init(file, 0x02 | 0x08 | 0x20, 0666, 0);
                foStream.write(data, data.length);
                foStream.close();
            }
        },
    }
    var metadata = {
        getTitle : function (item){
            var title = "";
            if (item.getProperty(SBProperties.trackName)!= null){
                title = item.getProperty(SBProperties.trackName);
            }else{
                title = "Unknown Track";
            }
            return title;
        },
        getArtist : function (item){
            var artist = "";
            if (item.getProperty(SBProperties.artistName)!=null){
                artist = item.getProperty(SBProperties.artistName);
            }else{
                artist = "Unknown Artist";
            }
            return artist;
        },
    }
    var myListener = {  
     onMediacoreEvent: function(ev) {  
         switch (ev.type) {  
             case Ci.sbIMediacoreEvent.TRACK_CHANGE:  
                 var mediaItem = gMM.sequencer.view.getItemByIndex(gMM.sequencer.viewPosition);
                 var tuneInfo = Utf8.encode(metadata.getTitle(mediaItem) + "\n"+ metadata.getArtist(mediaItem));
                 tuneFile.write(tuneInfo);
                 break;  
             case Ci.sbIMediacoreEvent.STREAM_START:  
                 var mediaItem = gMM.sequencer.view.getItemByIndex(gMM.sequencer.viewPosition);
                 var tuneInfo = Utf8.encode(metadata.getTitle(mediaItem) + "\n"+ metadata.getArtist(mediaItem));
                 tuneFile.write(tuneInfo);
                 break;  
             case Ci.sbIMediacoreEvent.STREAM_PAUSE:
                 tuneInfo = "";
                 tuneFile.write(tuneInfo);
                 break;
             case Ci.sbIMediacoreEvent.STREAM_END:
                 tuneInfo = "";
                 tuneFile.write(tuneInfo);
                 break;
             case Ci.sbIMediacoreEvent.STREAM_STOP:
                 tuneInfo = "";
                 tuneFile.write(tuneInfo);
                 break;
          }  
      }
    }
    gMM.addListener(myListener);

  },

  /**
   * Called when the window is about to close
   */
  onUnLoad: function() {
    gMM.removeListener(myListener)
    this._initialized = false;
  },

}

window.addEventListener("load", function(e) { Tunetopsi.Controller.onLoad(e); }, false);
window.addEventListener("unload", function(e) { Tunetopsi.Controller.onUnLoad(e); }, false);