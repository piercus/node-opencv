var cv = require('../lib/opencv');

cv.readImage('./files/mona.png', function(err, im) {
  if (err) throw err;
  if (im.width() < 1 || im.height() < 1) throw new Error('Image has no size');

  imgGray = im.copy();
  imgColor = im.copy();
  imgToHist = im.copy();

  im.save('./tmp/before_equalized.png');


  imgGray.convertGrayscale();

  imgGray.save('./tmp/gray_before_equalization.png');
  imgGray.equalizeHist();
  imgGray.save('./tmp/equalized.png');


  imgColor.cvtColor('CV_BGR2YCrCb');
  var channels = imgColor.split();
    /*console.log(
    cv.imgproc.calcHist(
      channels[0], 
      1, 
      null,
      new cv.Matrix(channels[0].height(),channels[0].width(),cv.Constants.CV_8UC1)
  ).col()
  );*/
  channels[0].equalizeHist();
/*
    console.log(
    cv.imgproc.calcHist(
      channels[0], 
      1, 
      null,
      new cv.Matrix(channels[0].height(),channels[0].width(),cv.Constants.CV_8UC1)
  ).col()
  );*/
  imgColor.merge(channels);
  imgColor.cvtColor('CV_YCrCb2BGR');

  imgColor.save('./tmp/color_equalized.png');

  console.log('Image saved to ./tmp/{before_equalized|gray_before_equalization|color_equalized|equalized}.png');
  /*
  console.log(channels[0],channels[0].width(),channels[0].height());

  console.log(
    cv.imgproc.calcHist(
      channels[0], 
      1,
      null,
      new cv.Matrix(channels[0].height(),channels[0].width(), cv.Constants.CV_8UC1)
  ).col()
  );*/

  imgToHist.cvtColor('CV_BGR2HSV');

  // Quantize the hue to 30 levels
  // and the saturation to 32 levels
  var hbins = 30, sbins = 32,
    histSize = [hbins, sbins],

    // hue varies from 0 to 179, see cvtColor
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    ranges = [[0,180],[0,256]],

    // we compute the histogram from the 0-th and 1-st channels
    channels = [0,1];

  var hist = cv.imgproc.calcHist(
    imgToHist, 1, channels, new cv.Matrix(), // do not use mask
    2, histSize, ranges,
    true, // the histogram is uniform
    false );
  
  var maxVal = 0;
  maxVal = hist.minMaxLoc().maxVal;

  var scale = 10;
  var histImg = new cv.Matrix(sbins*scale, hbins*10, cv.Constants.CV_8UC3);

  for( var h = 0; h < hbins; h++ ){
    for( var s = 0; s < sbins; s++ ){
      var binVal = hist.get(h, s);
      var intensity = Math.floor(binVal*255/maxVal);

      //console.log(intensity);

      histImg.rectangle([h*scale, s*scale],
        [(h+1)*scale - 1, (s+1)*scale - 1],
        [intensity, intensity, intensity],
        -1 //CV_FILLED
      );
    }
  }

  var window1 = new cv.NamedWindow("Source", 1);
  window1.show(im);

  var window2 = new cv.NamedWindow("H-S Histogram", 1);
  window2.show(histImg);

  window1.blockingWaitKey(0, 10000);


});
