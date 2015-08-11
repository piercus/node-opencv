var cv = require('../lib/opencv');

cv.readImage('./files/mona.png', function(err, im) {
  if (err) throw err;
  if (im.width() < 1 || im.height() < 1) throw new Error('Image has no size');

  img_gray = im.copy().convertGrayscale();

  img_gray.save('./tmp/gray_before_equalization.png');
  
  img_gray.equalizeHist().save('./tmp/equalized.png');

  console.log('Image saved to ./tmp/{gray_before_equalization|equalized}.png');
});
