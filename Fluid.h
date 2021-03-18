#pragma once
#include<SDL2/SDL.h>
#include <vector>
using namespace std;

class Fluid {

    private:

        int nElements = 0;
        int size;

		float amountAdded = 1.2f;		

        float diff = 0.01f;
	    float visc = 0.01f;
	    float dt = 0.005f;
    
	    //Geschwindigkeiten
	    vector<float>u;
	    vector<float>v;
	    vector<float>u0;
	    vector<float>v0;
	    //Dichte
	    vector<float>dens;
	    vector<float>dens0;

	    vector<float>pressure;
	    vector<float>div;

	    vector<float>source;

        void addSource(const vector<float>& s, vector<float>& x);
	    void setBounds(int N, int mode, vector<float>& x);
	    void diffuse(int N, int mode, vector<float>& x, const vector<float>& x0);
	    void advect(int N, int mode, vector<float>& dens, const vector<float>& dens0, const vector<float>& u, const vector<float>& v);
	    void project(int N, vector<float>& u, vector<float>& v, vector<float>& p, vector<float>& div);

    public:
        Fluid(int size);
		inline int IX(int i, int k) { return i + (size+  2) * k; }
		inline float map(float value, float min_in, float max_in, float min_out, double max_out) {
			return (value - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
		}		
	void linearSolve(int mode, vector<float> &x, const vector<float> &x0,float h1, float h2);
		void userInputSourceDensity(int xMouse, int yMouse);
		void userInputSourceVelocity(int xMouse, int yMouse);
        void calcVelocity(int size);
	    void calcDensity(int size);
		void draw(SDL_Renderer* renderer, int screenLength);

		inline float getDensContent(int i) { return dens[i]; }
		inline float getVelContent(int i) { return u[i]; }
}; 