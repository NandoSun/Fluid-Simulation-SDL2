#include "Fluid.h"
#include <vector>
#include <iostream>
//Test
Fluid::Fluid(int size)
	: size(size), nElements((2 + size) * (2 + size))
{
	

	for (int i = 0; i < nElements; i++)
	{
		u.push_back(0.0f);
		v.push_back(0.0f);
		u0.push_back(0.0f);
		v0.push_back(0.0f);
		dens.push_back(1.0f);
		dens0.push_back(1.0f);
		source.push_back(0.0f);
		pressure.push_back(1.0f);
		div.push_back(0.0f);
	}

	for (int i = dens0.size()  / 2; i < 30; i++){
		dens0[i] += 200;
	}
}

void Fluid::addSource(const vector<float> &source, vector<float> &x)
{
	for (int i = 0; i < nElements; i++)
	{
		x[i] += dt * source[i];
	}
}

void Fluid::setBounds(int N, int mode, vector<float> &x)
{

	for (int i = 1; i <= N; i++)
	{

		x[IX(0, i)] = mode == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = mode == 1 ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = mode == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = mode == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}

	x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

void Fluid::linearSolve(int mode, vector<float> &x, const vector<float> &x0, float h1, float h2)
{

	for (int k = 0; k < 20; k++)
	{

		for (int i = 1; i <= size; i++)
		{
			for (int j = 1; j <= size; j++)
			{

				x[IX(i, j)] = (x0[IX(i, j)] + h1 * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / (h2);
			}
		}
		setBounds(size, mode, x);
	}
}

void Fluid::diffuse(int N, int mode, vector<float> &x, const vector<float> &x0)
{
	float h = dt * diff * N * N;
	linearSolve(mode, x, x0, h, 1 + 4 * h + dt);
}

void Fluid::advect(int N, int mode, vector<float> &dens, const vector<float> &dens0, const vector<float> &u, const vector<float> &v)
{

	float x, y, s0, s1, t0, t1;
	int i0, i1, j0, j1;
	float dt0 = dt * N;

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{

			x = i - dt0 * u[IX(i, j)];
			y = j - dt0 * v[IX(i, j)];

			if (x < 0.5f)
				x = 0.5f;

			if (x > N + 0.5f)
				x = N + 0.5f;

			i0 = (int)x;
			i1 = i0 + 1;

			if (y < 0.5f)
				y = 0.5f;

			if (y > N + 0.5f)
				y = N + 0.5f;

			j0 = (int)y;
			j1 = j0 + 1;
			s1 = x - i0;
			s0 = 1.0f - s1;
			t1 = y - j0;
			t0 = 1.0f - t1;

			dens[IX(i, j)] = s0 * (t0 * dens0[IX(i0, j0)] + t1 * dens0[IX(i0, j1)]) + s1 * (t0 * dens0[i1, j0] + t1 * dens0[IX(i1, j1)]);
		}
	}
	setBounds(N, mode, dens);
}

void Fluid::project(int N, vector<float> &u, vector<float> &v, vector<float> &p, vector<float> &div)
{

	float h = 1.0f / N;

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{

			div[IX(i, j)] = -0.5f * h * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]);
			p[IX(i, j)] = 0.0f;
		}
	}
	setBounds(N, 0, div);
	setBounds(N, 0, p);
	linearSolve(0, p, div, 1, 4);

	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{

			u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / N;
			v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / N;
		}
	}
	setBounds(N, 1, u);
	setBounds(N, 2, v);
}

void Fluid::calcVelocity(int size)
{
	addSource(u, u0);
	addSource(v, v0);
	u0.swap(u);
	diffuse(size, 1, u, u0);
	v0.swap(v);
	diffuse(size, 2, v, v0);
	project(size, u, v, u0, v0);
	u0.swap(u);
	v0.swap(v);
	advect(size, 1, u, u0, u0, v0);
	advect(size, 2, v, v0, u0, v0);
	project(size, u, v, u0, v0);
}

void Fluid::calcDensity(int size)
{
	addSource(dens0, source);
	dens0.swap(dens);
	diffuse(size, 0, dens, dens0);
	dens0.swap(dens),
	advect(size, 0, dens, dens0, u, v);
}

void Fluid::draw(SDL_Renderer *renderer, int n)
{

	int nBoxes = n * n;
	SDL_RenderClear(renderer);
	for (int i = 0; i <= n + 1; i++)
	{
		for (int k = 0; k <= n + 1; k++)
		{

			SDL_Rect box;
			box.h = 640 / n;
			box.w = 640 / n;
			box.x = i * box.h;
			box.y = k * box.w;
			float absVal = sqrt(u[IX(i, k)] * u[IX(i, k)] + v[IX(i, k)] * v[IX(i, k)]);
			int color = (int)map(absVal, 1e-30, 1e-10, 1, 255);
			//std:: cout << color << endl;
			float color2 = map((dens[IX(i, k)]), 0, 1, 255, 1);
			//int color2 = (int)dens[IX(i, k)];

			SDL_SetRenderDrawColor(renderer, 150, color, 250, 255);
			SDL_RenderFillRect(renderer, &box);

			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
			SDL_RenderDrawRect(renderer, &box);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
		}
	}
	SDL_RenderPresent(renderer);
}

void Fluid::addVelRdn(){
	int velAngle, amount = 2000;
	srand(time(NULL));
	velAngle = rand() % 360;
	u0[u0.size() / 2] += cos(velAngle) * amount;
	v0[v0.size() / 2] += cos(360 - velAngle) * amount;
}

void Fluid::userInputSourceDensity(int xMouse, int yMouse)
{
	std::cout << "dens input\n";
	dens0[IX(xMouse / size, yMouse / size)] += amountAdded;
}

void Fluid::userInputSourceVelocity(int xMouse, int yMouse)
{
	std::cout << "velo input\n";
	u0[IX(xMouse / size, yMouse / size)] += amountAdded;
	v0[IX(xMouse / size, yMouse / size)] += amountAdded;
}
