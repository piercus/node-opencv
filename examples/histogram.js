//example translated from C++ from http://docs.opencv.org/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
var cv = require('../lib/opencv');

cv.readImage("./files/histoImg.jpg", function(err, im){
  if (err) throw err;
  if (im.width() < 1 || im.height() < 1) throw new Error('Image has no size');

  imgHist = im.copy();
  var planes = imgHist.split();

  var histSize = 256, histRange = [[0,256]], uniform = true;

  var bHist = cv.imgproc.calcHist(planes[0], 1, 0, new cv.Matrix(), 1, histSize, histRange, uniform);
  var gHist = cv.imgproc.calcHist(planes[1], 1, 0, new cv.Matrix(), 1, histSize, histRange, uniform);
  var rHist = cv.imgproc.calcHist(planes[2], 1, 0, new cv.Matrix(), 1, histSize, histRange, uniform);

  var histW = 512, histH = 400, binW = Math.round(histW/histSize);

  var histImage = new cv.Matrix(histH, histW, cv.Constants.CV_8UC3);

  bHist = cv.imgproc.normalize(bHist, 0, histImage.col().length, 32, -1, new cv.Matrix());
  gHist = cv.imgproc.normalize(gHist, 0, histImage.col().length, 32, -1, new cv.Matrix());
  rHist = cv.imgproc.normalize(rHist, 0, histImage.col().length, 32, -1, new cv.Matrix());

  for( var i = 1; i < histSize; i++ ){
      var p1 = [binW*(i-1), histH - Math.round(bHist.get(i-1)) ],
          p2 = [binW*(i), histH - Math.round(bHist.get(i))];
          
      histImage.line([binW*(i-1), histH - Math.round(bHist.get(i-1)) ],
                       [binW*(i), histH - Math.round(bHist.get(i))],
                       [255, 0, 0], 2, 8, 0  );
      histImage.line([binW*(i-1), histH - Math.round(gHist.get(i-1)) ],
                       [binW*(i), histH - Math.round(gHist.get(i))],
                       [0, 255, 0], 2, 8, 0  );
      histImage.line([binW*(i-1), histH - Math.round(rHist.get(i-1)) ],
                       [binW*(i), histH - Math.round(rHist.get(i))],
                       [0, 0, 255], 2, 8, 0  );
  }

  var window1 = new cv.NamedWindow("calcHist Demo", 1);
  window1.show(histImage);

  window1.blockingWaitKey(0, 3000);

});
