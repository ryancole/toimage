
var toimage = require('../lib/toimage');

// convert a single document, using default settings
toimage.convert('./test/etc/foo.xlsx', './test/etc/foo.png', function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});