var cv = require('../lib/opencv');

cv.readImage('./files/mona.png', function(err, im) {
  if (err) throw err;
  if (im.width() < 1 || im.height() < 1) throw new Error('Image has no size');

  img_gray = im.copy();
  img_color = im.copy();

  im.save('./tmp/before_equalized.png');


  img_gray.convertGrayscale();

  img_gray.save('./tmp/gray_before_equalization.png');
  img_gray.equalizeHist();
  img_gray.save('./tmp/equalized.png');


  img_color.cvtColor('CV_BGR2YCrCb');
  var channels = img_color.split();
  channels[0].equalizeHist();
  img_color.merge(channels);
  img_color.cvtColor('CV_YCrCb2BGR');

  img_color.save('./tmp/color_equalized.png');

  console.log('Image saved to ./tmp/{before_equalized|gray_before_equalization|color_equalized|equalized}.png');



});
