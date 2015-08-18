//example translated from C++ from https://github.com/Itseez/opencv/blob/master/samples/cpp/lkdemo.cpp
var cv = require('../lib/opencv');
var path = require('path');

var help = function(){
    // print a welcome message, and the OpenCV version
    console.log("\nThis is a node-demo of Lukas-Kanade optical flow lkdemo(),\n"+
            "Using OpenCV version " + cv.Constants.CV_VERSION +
            "\nIt uses camera by default, but you can provide a path to video as an argument.\n"+
            "\nHot keys: \n"+
            "\tESC - quit the program\n"+
            "\tr - auto-initialize tracking\n"+
            "\tc - delete all the points\n"+
            "\tn - switch the \"night\" mode on/off\n"+
            "To add/remove a feature point click it\n");
}

var point;
var addRemovePt = false; 

var onMouse = function(event, x, y, flags, param){
    if( event == cv.Constants.EVENT_LBUTTONDOWN ){
        point = new cv.Point2f(x, y);
        addRemovePt = true;
    }
}


help();

//VideoCapture cap;
//TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
//Size subPixWinSize(10,10), winSize(31,31);

//console.log(cv);

var termCriteria = new cv.TCriteria(
    cv.Constants.CV_TERMCRIT_ITER|cv.Constants.CV_TERMCRIT_EPS, 
    20, 
    0.03 
);

var MAX_COUNT = 500;
var needToInit = true;
var nightMode = false;
var subPixWinSize = [10,10], winSize = [31,31];

//if(process.argv.length == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
//    cap.open(argc == 2 ? argv[1][0] - '0' : 0);
//else if( argc == 2 )
//    cap.open(argv[1]);

var vid = new cv.VideoCapture(path.join(__dirname, 'files', 'motion.mov'));
var window = new cv.NamedWindow("LK Demo", 1);

//vector<Point2f> points[2];
var points = new Array(2);

while(1){


    var frame = vid.readSync();

    if(frame.empty()){
        console.log("End of the video");
        return;
    }

    var image = frame.copy();
    image.cvtColor('CV_BGR2YCrCb');
    var channels = image.split(), 
        gray = channels[0];

    if( nightMode ){
        //image = [0,0,0];
    }

    if( needToInit ){
        // automatic initialization
        /*console.log("image start", 
            gray.get(0,0), 
            gray.get(0,1), 
            gray.get(1,0), 
            gray.get(1,1));
*/
        var mask = new cv.Matrix(0,0,cv.Constants.CV_8UC1);

        points[1] = gray.goodFeaturesToTrack(MAX_COUNT, 0.01, 10, mask, 3, 0, 0.04);
        
        //console.log("points[1][1] : ", points[1][1]);
        //return;

        points[1] = gray.cornerSubPix(points[1], subPixWinSize, [-1,-1], termCriteria);
        //console.log("after cornerSubPix points[1][1] : ", points[1]);
        //return


        addRemovePt = false;
    } else if(points[0] && points[0].length && points[0].length > 0){
        //vector<uchar> status;
        //vector<float> err;
        
        if(prevGray.empty()){
            prevGray = gray.copyTo();
        }
        /*console.log("grays points\n", 
            gray.get(0,0), 
            gray.get(0,1), 
            gray.get(1,0), 
            gray.get(1,1));
        console.log("\n============bf optical call============\n", points[0][1], prevGray, gray, winSize, termCriteria);*/


        var result = cv.opticalFlow.calcOpticalFlowPyrLK(
            prevGray, 
            gray, 
            points[0],  
            winSize,
            3, 
            termCriteria, 
            0, 
            0.001
        );
        
        //console.log("after opticalFlow", points[1]);

        points[1] = result.nextPts;
        status = result.status;
        errors = result.err;
        
        //TO DO : error detection
        //if(errors.length > 0){
            //console.log("Errors happened in Optical Flow" + errors);
            //throw new Error();
        //}

        var i, k, l = points[1] ? points[1].length : 0;
        //console.log("l ",l);
        for( i = k = 0; i < l; i++ ) {
            if( addRemovePt ) {
                if( norm(point - points[1].get(i,0)) <= 5 )
                {
                    addRemovePt = false;
                    continue;
                }
            }
            if( !status[i] )
                continue;

            //console.log(i, status[i]);


            points[1][k][0] = points[1][i][0];
            points[1][k][1] = points[1][i][1];

            /*console.log([
                points[1][i][0],
                points[1][i][1]
                ]);*/
            k++;

            frame.circle( [
                points[1][i][0],
                points[1][i][1]
                ], 3, [0, 255, 0], -1, 8);
        }
        //console.log("k  is  ",k);

        points[k] = points[1].slice(0,k);
        //console.log("after resize");
    }

    if( addRemovePt && points[1].length() < MAX_COUNT ){
        var tmp;
        tmp.push(point);
        winSize = cv.cornerSubPix( gray, tmp, [-1,-1], termCriteria);
        points[1].push(tmp[0]);
        addRemovePt = false;
    }

    needToInit = false;
    window.show(frame);
    window.blockingWaitKey(0, 10);
    /*
    window.waitKey(function(c){
        if( c == 27 ){
                //to do break
        }
            

        switch(c){
            case 'r':
                needToInit = true;
                break;
            case 'c':
                points[0] = null;
                points[1] = null;
                break;
            case 'n':
                nightMode = !nightMode;
                break;
        }
    });*/

    points[0] = points[1];
    prevGray = gray;
}
