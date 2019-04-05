
#include "stats.h"

stats::stats(PNG & im){
	int width = im.width();
	int height = im.height();

	//Per column vectors that will be pushed into final vectors
	vector<double> colSumHueX;
	vector<double> colSumHueY;
	vector<double> colSumSat;
	vector<double> colSumLum;

	//Vector of each pixel's histogram in this column
	vector<vector<int>> colHist;

	//Just this column's histogram
	vector<int> cleanHist(36, 0);
	vector<int> pixHist(36, 0);
	vector<int> placedHist;

	double hueX = 0;
	double hueY = 0;
	double sat = 0;
	double lum = 0;
	double hue;

	HSLAPixel* currentPix;

	for (int j = 0; j < height; j++){
		currentPix = im.getPixel(0, j);
		hue = currentPix->h;
		hueX += cos(hue * PI / 180);
		hueY += sin(hue * PI / 180);
		sat += currentPix->s;
		lum += currentPix->l;

		colSumHueX.push_back(hueX);
		colSumHueY.push_back(hueY);
		colSumSat.push_back(sat);
		colSumLum.push_back(lum);

		pixHist[hue / 10]++;
		placedHist = pixHist;
		colHist.push_back(placedHist);

	}

	sumHueX.push_back(colSumHueX);
	sumHueY.push_back(colSumHueY);
	sumSat.push_back(colSumSat);
	sumLum.push_back(colSumLum);
	hist.push_back(colHist);



	for (int i = 1; i < width; i++){
		double hueX = 0;
		double hueY = 0;
		double sat = 0;
		double lum = 0;

		colSumHueX.clear();
		colSumHueY.clear();
		colSumSat.clear();
		colSumLum.clear();
		colHist.clear();

		pixHist.clear();
		pixHist = cleanHist;


		for (int j = 0; j < height; j++){
			currentPix = im.getPixel(i, j);
			hue = currentPix->h;
			hueX += cos(hue * PI / 180);
			hueY += sin(hue * PI / 180);
			sat += currentPix->s;
			lum += currentPix->l;

			colSumHueX.push_back(hueX + sumHueX[i-1][j]);
			colSumHueY.push_back(hueY + sumHueY[i-1][j]);
			colSumSat.push_back(sat + sumSat[i-1][j]);
			colSumLum.push_back(lum + sumLum[i-1][j]);

			pixHist[hue / 10]++;
			placedHist = hist[i-1][j];
			for (int k = 0; k < 36; k++){
				placedHist[k] += pixHist[k];
			}
			colHist.push_back(placedHist);
		}

		sumHueX.push_back(colSumHueX);
		sumHueY.push_back(colSumHueY);
		sumSat.push_back(colSumSat);
		sumLum.push_back(colSumLum);
		hist.push_back(colHist);

	}


}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
	int x = lr.first - ul.first + 1;
	int y = lr.second - ul.second + 1;
	return (long) x * y;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
	HSLAPixel returnPix = HSLAPixel();
	long numPix = rectArea(ul, lr);
	double avgY,  avgX;

	int x1 = ul.first;
	int x2 = lr.first;
	int y1 = ul.second;
	int y2 = lr.second;


	if (x1 == 0 && y1 == 0){
		avgY = sumHueY[x2][y2];
		avgX = sumHueX[x2][y2];
		returnPix.h = radToDeg(avgY,avgX);
		returnPix.l = sumLum[x2][y2]/ numPix;
		returnPix.s = sumSat[x2][y2]/ numPix;
	} else if (x1 == 0){
		avgY = (sumHueY[x2][y2] - sumHueY[x2][y1 - 1]);
		avgX = (sumHueX[x2][y2] - sumHueX[x2][y1 - 1]);
		returnPix.h = radToDeg(avgY,avgX);
		returnPix.l = (sumLum[x2][y2] - sumLum[x2][y1 - 1])/ numPix;
		returnPix.s = (sumSat[x2][y2] - sumSat[x2][y1 - 1])/ numPix;
	} else if (y1 == 0){
		avgY = (sumHueY[x2][y2] - sumHueY[x1 - 1][y2]);
		avgX = (sumHueX[x2][y2] - sumHueX[x1 - 1][y2]);
		returnPix.h = radToDeg(avgY,avgX);
		returnPix.l = (sumLum[x2][y2] - sumLum[x1 - 1][y2])/ numPix;
		returnPix.s = (sumSat[x2][y2] - sumSat[x1 - 1][y2])/ numPix;
	} else{
		avgY = (sumHueY[x2][y2] - sumHueY[x1 - 1][y2] - sumHueY[x2][y1 - 1] + sumHueY[x1 - 1][y1 - 1]);
		avgX = (sumHueX[x2][y2] - sumHueX[x1 - 1][y2] - sumHueX[x2][y1 - 1] + sumHueX[x1 - 1][y1 - 1]);
		returnPix.h = radToDeg(avgY,avgX);
		returnPix.l = (sumLum[x2][y2] - sumLum[x1 - 1][y2] - sumLum[x2][y1 - 1] + sumLum[x1 - 1][y1 - 1]) / numPix;
		returnPix.s = (sumSat[x2][y2] - sumSat[x1 - 1][y2] - sumSat[x2][y1 - 1] + sumSat[x1 - 1][y1 - 1]) / numPix;
	}


	return returnPix;
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
	vector<int> returnHist(36, 0);

	int x1 = ul.first;
	int x2 = lr.first;
	int y1 = ul.second;
	int y2 = lr.second;

	if (x1 == 0 && y1 == 0){
		returnHist = hist[x2][y2];
	}
	else if (x1 == 0){
		returnHist = subHist(hist[x2][y2], hist[x2][y1 - 1]);
	}
	else if (y1 == 0){
		returnHist = subHist(hist[x2][y2], hist[x1 - 1][y2]);
	}
	else{
		returnHist = subHist( addHist(hist[x2][y2], hist[x1 - 1][y1 - 1]), addHist(hist[x2][y1 - 1], hist[x1 - 1][y2]));
	}

	return returnHist;

}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn, int area){

    double entropy = 0.0;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
	vector<int> dist = buildHist(ul, lr);

	int area = rectArea(ul, lr);

	return entropy(dist, area);
}


vector<int> stats::addHist(vector<int> hist1, vector<int> hist2){
	vector<int> returnHist(36, 0);

	for (int i = 0; i < 36; i++){
		returnHist[i] = hist1[i] + hist2[i];
	}

	return returnHist;
}

vector<int> stats::subHist(vector<int> hist1, vector<int> hist2){
	vector<int> returnHist(36, 0);

	for (int i = 0; i < 36; i++){
		returnHist[i] = hist1[i] - hist2[i];
	}

	return returnHist;
}

double stats::radToDeg(double x, double y){
	double deg = atan2(y, x) * 180 / PI;
	
	if (deg < 0){
		deg += 360;
	}

	return deg;

}