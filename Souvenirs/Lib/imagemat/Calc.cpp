#include <math.h>
#include "nrutil.h"
#include "Calc.h"
#include "time.h"

#include <vector>
using namespace std;
//Computes (a2 + b2)1/2 without destructive underflow or overflow.
double pythag(double a, double b)
{
	double absa, absb;
	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) {
		return absa * sqrt(1.0 + SQR(absb / absa));
	} else {
		return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
	}
}

// Given a matrix a[1..m][1..n], this routine computes its singular value decomposition, A =
// U¡¦W¡¦V T. Thematrix U replaces a on output. The diagonal matrix of singular values W is output
// as a vector w[1..n]. Thematrix V (not the transpose V T ) is output as v[1..n][1..n].
void svdcmp(double **a, int m, int n, double w[], double **v)
{
	int     flag, i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	double *rv1;
	rv1 = dvector(1, n);
	g = scale = anorm = 0.0; // Householder reduction to bidiagonal form.
	for (i = 1; i <= n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m) {
			for (k = i; k <= m; k++) {
				scale += fabs(a[k][i]);
			}
			if (scale) {
				for (k = i; k <= m; k++) {
					a[k][i] /= scale;
					s += a[k][i] * a[k][i];
				}
				f=a[i][i];
				g = (double)(-SIGN(sqrt(s), f));
				h=f * g - s;
				a[i][i] = f - g;
				for (j = l; j <= n; j++) {
					for (s=0.0,k=i;k<=m;k++) {
						s += a[k][i]*a[k][j];
					}
					f = s / h;
					for (k = i; k <= m; k++) {
						a[k][j] += f*a[k][i];
					}
				}
				for (k = i; k <= m; k++) {
					a[k][i] *= scale;
				}
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n) {
			for (k = l; k <= n; k++) {
				scale += (double)fabs(a[i][k]);
			}
			if (scale) {
				for (k = l; k <= n; k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f = a[i][l];
				g = (double)(-SIGN(sqrt(s), f));
				h = f * g - s;
				a[i][l] = f - g;
				for (k = l; k <= n; k++) {
					rv1[k] = a[i][k] / h;
				}
				for (j = l; j <= m; j++) {
					for (s = 0.0, k = l; k <= n; k++) {
						s += a[j][k]*a[i][k];
					}
					for (k = l; k <= n; k++) {
						a[j][k] += s * rv1[k];
					}
				}
				for (k = l; k <= n; k++) {
					a[i][k] *= scale;
				}
			}
		}
		anorm = (double)FMAX(anorm, ((double)fabs(w[i]) + (double)fabs(rv1[i])));
	}
	for (i = n; i >= 1; i--) {
		//Accumulation of right-hand transformations.
		if (i < n) {
			if (g) {
				for (j = l; j <= n; j++) {
					// Double division to avoid possible underflow.
					v[j][i] = (a[i][j] / a[i][l]) / g;
				}
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = l; k <= n; k++) {
						s += a[i][k]*v[k][j];
					}
					for (k = l; k <= n; k++) {
						v[k][j] += s * v[k][i];
					}
				}
			}
			for (j = l; j <= n; j++) {
				v[i][j] = v[j][i] = 0.0;
			}
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = IMIN(m, n); i >= 1; i--) {
		//Accumulation of left-hand transformations.
		l = i + 1;
		g = w[i];
		for (j = l; j <= n; j++) {
			a[i][j] = 0.0;
		}
		if (g) {
			g = (double)1.0 / g;
			for (j = l; j <= n; j++) {
				for (s = 0.0, k = l; k <= m; k++) {
					s += a[k][i] * a[k][j];
				}
				f = (s / a[i][i]) * g;
				for (k = i; k <= m; k++) {
					a[k][j] += f * a[k][i];
				}
			}
			for (j = i; j <= m; j++) {
				a[j][i] *= g;
			}
		} else {
			for (j = i; j <= m; j++) {
				a[j][i] = 0.0;
			}
		}
		++a[i][i];
	}
	for (k = n; k >= 1; k--) {
		//Diagonalization of the bidiagonal form: Loop oversingular values, and over allowed iterations.
		for (its = 1; its <= 30; its++) {			
			flag = 1;
			for (l = k; l >= 1; l--) { 
				//Test for splitting.
				nm = l - 1; // Note that rv1[1] is always zero.
				if ((double)(fabs(rv1[l]) + anorm) == anorm) {
					flag = 0;
					break;
				}
				if ((double)(fabs(w[nm]) + anorm) == anorm) {
					break;
				}
			}
			if (flag) {
				c = 0.0; //Cancellation of rv1[l], if l > 1.
				s=1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if ((double)(fabs(f) + anorm) == anorm) {
						break;
					}
					g = w[i];
					h = pythag(f, g);
					w[i] = h;
					h = (double)1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 1; j <= m; j++) {
						y = a[j][nm];
						z=a[j][i];
						a[j][nm] = y * c + z * s;
						a[j][i] = z * c - y * s;
					}
				}
			}
			z = w[k];
			if (l == k) {
				//Convergence.
				if (z < 0.0) {
					//Singular value is made nonnegative.
					w[k] = -z;
					for (j = 1; j <= n; j++) {
						v[j][k] = -v[j][k];
					}
				}
				break;
			}
			if (its == 30) {
				nrerror("no convergence in 30 svdcmp iterations");
			}
			x = w[l]; //Shift from bottom 2-by-2 minor.
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = (double)(((y - z) * (y + z) + (g - h) * (g + h))/(2.0 * h * y));
			g = pythag(f, 1.0);
			f = (double)((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
			c = s = 1.0; // Next QR transformation:
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 1; jj <= n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x * c + z * s;
					v[jj][i] = z * c - x * s;
				}
				z = pythag(f, h);
				w[j] = z;

				// Rotation can be arbitrary if z = 0.
				if (z) {
					z = (double)(1.0 / z);
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;				
				for (jj = 1; jj <= m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y * c + z * s;
					a[jj][i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	free_dvector(rv1, 1, n);
}

void svdcmp(float **a, int m, int n, double w[], float **v)
{
	int     flag, i, its, j, jj, k, l, nm;
	double anorm, c, f, g, h, s, scale, x, y, z;
	double *rv1;
	rv1 = dvector(1, n);
	g = scale = anorm = 0.0; // Householder reduction to bidiagonal form.
	for (i = 1; i <= n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m) {
			for (k = i; k <= m; k++) {
				scale += fabs(a[k][i]);
			}
			if (scale) {
				for (k = i; k <= m; k++) {
					a[k][i] = (float)(a[k][i] / scale);
					s += a[k][i] * a[k][i];
				}
				f=a[i][i];
				g = (double)(-SIGN(sqrt(s), f));
				h=f * g - s;
				a[i][i] = (float)(f - g);
				for (j = l; j <= n; j++) {
					for (s=0.0,k=i;k<=m;k++) {
						s += a[k][i]*a[k][j];
					}
					f = s / h;
					for (k = i; k <= m; k++) {
						a[k][j]= (float)(a[k][j] + f*a[k][i]);
					}
				}
				for (k = i; k <= m; k++) {
					a[k][i]= (float)(a[k][i] * scale);
				}
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n) {
			for (k = l; k <= n; k++) {
				scale += (double)fabs(a[i][k]);
			}
			if (scale) {
				for (k = l; k <= n; k++) {
					a[i][k] = (float)(a[i][k] / scale);
					s += a[i][k]*a[i][k];
				}
				f = a[i][l];
				g = (double)(-SIGN(sqrt(s), f));
				h = f * g - s;
				a[i][l] = (float)(f - g);
				for (k = l; k <= n; k++) {
					rv1[k] = a[i][k] / h;
				}
				for (j = l; j <= m; j++) {
					for (s = 0.0, k = l; k <= n; k++) {
						s += a[j][k]*a[i][k];
					}
					for (k = l; k <= n; k++) {
						a[j][k] = (float)(a[j][k] + s * rv1[k]);
					}
				}
				for (k = l; k <= n; k++) {
					a[i][k] = (float)(a[i][k] * scale);
				}
			}
		}
		anorm = (double)FMAX(anorm, ((double)fabs(w[i]) + (double)fabs(rv1[i])));
	}
	for (i = n; i >= 1; i--) {
		//Accumulation of right-hand transformations.
		if (i < n) {
			if (g) {
				for (j = l; j <= n; j++) {
					// Double division to avoid possible underflow.
					v[j][i] = (float)((a[i][j] / a[i][l]) / g);
				}
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = l; k <= n; k++) {
						s += a[i][k]*v[k][j];
					}
					for (k = l; k <= n; k++) {
						v[k][j] = (float)(v[k][j] + s * v[k][i]);
					}
				}
			}
			for (j = l; j <= n; j++) {
				v[i][j] = v[j][i] = 0.0;
			}
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = IMIN(m, n); i >= 1; i--) {
		//Accumulation of left-hand transformations.
		l = i + 1;
		g = w[i];
		for (j = l; j <= n; j++) {
			a[i][j] = 0.0;
		}
		if (g) {
			g = (double)1.0 / g;
			for (j = l; j <= n; j++) {
				for (s = 0.0, k = l; k <= m; k++) {
					s += a[k][i] * a[k][j];
				}
				f = (s / a[i][i]) * g;
				for (k = i; k <= m; k++) {
					a[k][j] = (float)(a[k][j] + f * a[k][i]);
				}
			}
			for (j = i; j <= m; j++) {
				a[j][i] = (float)(a[j][i] * g);
			}
		} else {
			for (j = i; j <= m; j++) {
				a[j][i] = 0.0;
			}
		}
		++a[i][i];
	}
	for (k = n; k >= 1; k--) {
		//Diagonalization of the bidiagonal form: Loop oversingular values, and over allowed iterations.
		for (its = 1; its <= 30; its++) {			
			flag = 1;
			for (l = k; l >= 1; l--) { 
				//Test for splitting.
				nm = l - 1; // Note that rv1[1] is always zero.
				if ((double)(fabs(rv1[l]) + anorm) == anorm) {
					flag = 0;
					break;
				}
				if ((double)(fabs(w[nm]) + anorm) == anorm) {
					break;
				}
			}
			if (flag) {
				c = 0.0; //Cancellation of rv1[l], if l > 1.
				s=1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if ((double)(fabs(f) + anorm) == anorm) {
						break;
					}
					g = w[i];
					h = pythag(f, g);
					w[i] = h;
					h = (double)1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 1; j <= m; j++) {
						y = a[j][nm];
						z=a[j][i];
						a[j][nm] = (float)(y * c + z * s);
						a[j][i] = (float)(z * c - y * s);
					}
				}
			}
			z = w[k];
			if (l == k) {
				//Convergence.
				if (z < 0.0) {
					//Singular value is made nonnegative.
					w[k] = -z;
					for (j = 1; j <= n; j++) {
						v[j][k] = -v[j][k];
					}
				}
				break;
			}
			if (its == 30) {
				nrerror("no convergence in 30 svdcmp iterations");
			}
			x = w[l]; //Shift from bottom 2-by-2 minor.
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = (double)(((y - z) * (y + z) + (g - h) * (g + h))/(2.0 * h * y));
			g = pythag(f, 1.0);
			f = (double)((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
			c = s = 1.0; // Next QR transformation:
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 1; jj <= n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = (float)(x * c + z * s);
					v[jj][i] = (float)(z * c - x * s);
				}
				z = pythag(f, h);
				w[j] = z;

				// Rotation can be arbitrary if z = 0.
				if (z) {
					z = (double)(1.0 / z);
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;				
				for (jj = 1; jj <= m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = (float)(y * c + z * s);
					a[jj][i] = (float)(z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	free_dvector(rv1, 1, n);
}

#define TINY 1.0e-20

// Given a matrix a[1..n][1..n], this routine replaces it by the LU decomposition of a rowwise
// permutation of itself. a and n are input. a is output, arranged as in equation (2.3.14) above;
// indx[1..n] is an output vector that records the row permutation effected by the partial
// pivoting; d is output as ¢£1 depending on whether the number of row interchanges was even
// or odd, respectively. This routine is used in combination with lubksb to solve linear equations
// or invert a matrix.

void ludcmp(double **a, int n, int *indx, double *d)
{
	int i, imax, j, k;
	double big, dum, sum, temp;
	double *vv; //vv stores the implicit scaling of each row.
	
	vv = dvector(1, n);
	*d = 1.0; //No row interchanges yet.
	for (i = 1; i <= n; i++) {
		big= 0.0; 
		//Loop over rows to get the implicit scaling information.
		for (j = 1; j <= n; j++) {
			if ((temp = (double)fabs(a[i][j])) > big) big = temp;
		}
		if (big == 0.0) {
			nrerror("Singular matrix in routine ludcmp");
		}
		//No nonzero largest element.
		vv[i] = (double)(1.0 / big); //Save the scaling.
	}
	for (j = 1; j <= n; j++) {
		//This is the loop over columns of Crout¡Çs method.
		for (i = 1; i < j; i++) {
			//This is equation (2.3.12) except for i = j.
			sum = a[i][j];
			for (k = 1; k < i; k++) {
				sum -= a[i][k] * a[k][j];
			}
			a[i][j]=sum;
		}
		big = 0.0; //Initialize for the search for largest pivot element.
		for (i = j; i <= n; i++) {
			//This is i = j of equation (2.3.12) and i = j+1. . .N  of equation (2.3.13).
			sum = a[i][j]; 
			for (k = 1; k < j; k++) {
				sum -= a[i][k] * a[k][j];
			}
			a[i][j] = sum;
			if ( (dum = vv[i] * (double)fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			//Do we need to interchange rows?
			for (k = 1; k <= n; k++) { 
				//Yes, do so...
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d); //...and change the parity of d.
			vv[imax] = vv[j]; //Also interchange the scale factor.
		}
		indx[j] = imax;
		if (a[j][j] == 0.0) {
			a[j][j] = (double)TINY;
		}
// 		If the pivot element is zero the matrix is singular (at least to the precision of the algorithm).
//		For some applications on singular matrices, it is desirable to substitute TINY for zero.

		if (j != n) {
			//Now, finally, divide by the pivot element.
			dum = (double)(1.0 / (a[j][j]));
			for (i = j + 1; i <= n; i++) {
				a[i][j] *= dum;
			}
		}
	}
	free_dvector(vv, 1, n);	
}

// Solves the set of n linear equations A¡¦X = B. Here a[1..n][1..n] is input, not as the matrix A but rather as its LU decomposition, determined by the routine ludcmp. indx[1..n] is input
// as the permutation vector returned by ludcmp. b[1..n] is input as the right-hand side vector B, and returns with the solution vector X. a, n, and indx are not modified by this routine
// and can be left in place for successive calls with different right-hand sides b. This routine takes into account the possibility that b will begin with many zero elements, so it is efficient for use in matrix inversion.

void lubksb(double **a, int n, int *indx, double b[])
{
	int i, ii = 0, ip, j;
	double sum;
	for (i = 1; i <= n; i++) {
// 		When ii is set to a positive value, it will become the
// 		index of the first nonvanishing element of b. We now do the forward substitution, equation (2.3.6). The	only new wrinkle is to unscramble the permutation as we go.

		ip = indx[i]; sum = b[ip]; b[ip] = b[i];
		if (ii) {
			for (j = ii; j <= i - 1; j++) {
				sum -= a[i][j] * b[j];
			}
		} else if (sum) {
			ii = i;
		}
		b[i] = sum;  //A nonzero element was encountered, so from now on we will  have to do the sums in the loop above.
	}
	for (i = n; i >= 1; i--) {
		//Now we do the backsubstitution, equation (2.3.7).
		sum = b[i];
		for (j = i + 1; j <= n; j++) {
			sum -= a[i][j] * b[j];
		}
		b[i] = sum / a[i][i]; //Store a component of the solution vector X.
	} 
}

void LinearSolve(double **a, int n, double b[])
{
	double d;
	int     *indx;	
	indx  = ivector(1, n);
	ludcmp(a, n, indx, &d);
	lubksb(a, n, indx, b);
	free_ivector(indx, 1, n);
}

void InvMatrix(double **a, int n, double **y)
{
	double d, *col;
	int i, j, *indx;
	indx = ivector(1, n);
	col = dvector(1, n);
	ludcmp(a, n, indx, &d); //Decompose the matrix just once.
	for(j = 1; j <= n; j++) {
		//Find inverse by columns.
		for(i = 1; i <= n; i++) {
			col[i] = 0.0;
		}
		col[j] = 1.0;
		lubksb(a, n, indx, col);
		for(i = 1; i <= n; i++) {
			y[i][j] = col[i];
		}
	}
	free_ivector(indx, 1, n);
	free_dvector(col, 1, n);	
}

void InvMatrix(double **a, int n)
{
	double d, *col;
	int i, j, *indx;
	double **y;
	y = dmatrix(1, n, 1, n);
	indx = ivector(1, n);
	col = dvector(1, n);
	ludcmp(a, n, indx, &d); //Decompose the matrix just once.
	for(j = 1; j <= n; j++) {
		//Find inverse by columns.
		for(i = 1; i <= n; i++) {
			col[i] = 0.0;
		}
		col[j] = 1.0;
		lubksb(a, n, indx, col);
		for(i = 1; i <= n; i++) {
			y[i][j] = col[i];
		}
	}
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] = y[i][j];
		}
	}
	free_ivector(indx, 1, n);
	free_dvector(col, 1, n);
	free_dmatrix(y, 1, n, 1, n);
}

double Determination(double **a, int n)
{
	double d;
	int *indx, j;
	indx = ivector(1, n);	
	ludcmp(a, n, indx, &d);
	for(j = 1; j <= n; j++) {
		d *= a[j][j];
	}
	free_ivector(indx, 1, n);	
	return d;
}

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
a[k][l]=h+s*(g-h*tau);

// Computes all eigenvalues and eigenvectors of a real symmetric matrix a[1..n][1..n]. 
//On output, elements of a above the diagonal are destroyed. d[1..n] returns the eigenvalues of a.
// v[1..n][1..n] is a matrix whose columns contain, on output, the normalized eigenvectors of a. 
//nrot returns the number of Jacobi rotations that were required.
void jacobi(double **a, int n, double d[], double **v, int *nrot)
{
	int j, iq, ip, i;
	double tresh, theta, tau, t, sm, s, h, g, c, *b, *z;

	b = dvector(1, n);
	z = dvector(1, n);
	//Initialize to the identity matrix.
	for (ip = 1; ip <= n; ip++) { 		
		for (iq = 1; iq <= n; iq++) {
			v[ip][iq] = 0.0;
		}
		v[ip][ip] = 1.0;
	}
	for (ip = 1; ip <= n; ip++) {
		//Initialize b and d to the diagonal of a.
		b[ip] = d[ip] = a[ip][ip]; 
		z[ip] = 0.0; //This vector will accumulate terms of the form tapq as in equation (11.1.14).
	}
	*nrot = 0;
	for (i = 1; i <= 50; i++) {
		sm = 0.0;
		for (ip = 1; ip <= n - 1; ip++) {
			//Sum off-diagonal elements.
			for (iq = ip + 1; iq <= n; iq++) {
				sm += (double)fabs(a[ip][iq]);
			}
		}
		if (sm == 0.0) {
			//The normal return, which relies on quadratic convergence to machine underflow.			
			free_dvector(z, 1, n);
			free_dvector(b, 1, n);
			return;
		}
		if (i < 4) {
			//...on the first three sweeps.
			tresh = (double)(0.2 * sm / (n * n)); 
		} else {
			tresh=0.0; //...thereafter.
		}
		for (ip = 1; ip <= n - 1; ip++) {
			for (iq = ip + 1; iq <= n; iq++) {
				g = (double)(100.0 * fabs(a[ip][iq]));
				//After four sweeps, skip the rotation if the off-diagonal element is small.
				if (i > 4 && (double)(fabs(d[ip]) + g) == (double)fabs(d[ip]) 
					&& (double)(fabs(d[iq]) + g) == (double)fabs(d[iq])) {
					a[ip][iq]=0.0;
				} else if (fabs(a[ip][iq]) > tresh) {
					h = d[iq] - d[ip];
					if ((double)(fabs(h) + g) == (double)fabs(h)) {
						t = (a[ip][iq]) / h; 
					} else {
						theta = (double)(0.5 * h / (a[ip][iq])); //Equation (11.1.10).
						t = (double)(1.0 / (fabs(theta) + sqrt(1.0 + theta * theta)));
						if (theta < 0.0) {
							t = -t;
						}
					}
					c = (double)(1.0 / sqrt(1 + t * t));
					s = t * c;
					tau = (double)(s / (1.0 + c));
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.0;
					for (j = 1; j <= ip - 1; j++) {
						//Case of rotations 1 . j < p.
						ROTATE(a, j, ip, j, iq)
					}
					for (j = ip + 1; j <= iq - 1; j++) {
						//Case of rotations p < j < q.
						ROTATE(a, ip, j, j, iq)
					}
					for (j = iq + 1; j <= n; j++) {
						//Case of rotations q < j . n.
						ROTATE(a, ip, j, iq, j)
					}
					for (j = 1; j <= n; j++) {
						ROTATE(v, j, ip, j, iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip = 1; ip <= n; ip++) {
			b[ip] += z[ip];
			d[ip] = b[ip]; //Update d with the sum of tapq,
			z[ip] = 0.0;   //and reinitialize z.
		}
	}
	eigsrt(d, v, n);
}

// Given the eigenvalues d[1..n] and eigenvectors v[1..n][1..n] as output from jacobi
// (¢Û11.1) or tqli (¢Û11.3), this routine sorts the eigenvalues into descending order, and rearranges
// the columns of v correspondingly. The method is straight insertion.

void eigsrt(double d[], double **v, int n)
{
	int k, j, i;
	double p;
	for (i = 1; i < n; i++) {
		p = d[k = i];
		for (j = i + 1; j <= n; j++) {
			if (d[j] >= p) {
				p = d[k = j];
			}
		}
		if (k != i) {
			d[k] = d[i];
			d[i] = p;
			for (j = 1; j <= n; j++) {
				p = v[j][i];
				v[j][i] = v[j][k];
				v[j][k] = p;
			}
		}
	}
}

// Householder reduction of a real, symmetric matrix a[1..n][1..n]. On output, a is replaced
// by the orthogonal matrix Q effecting the transformation. d[1..n] returns the diagonal elements
// of the tridiagonal matrix, and e[1..n] the off-diagonal elements, with e[1]=0. Several
// statements, as noted in comments, can be omitted if only eigenvalues are to be found, in which
// case a contains no useful information on output. Otherwise they are to be included.

void tred2(double **a, int n, double d[], double e[])
{
	int l, k, j, i;
	double scale, hh, h, g, f;
	for (i = n; i >= 2; i--) {
		l = i - 1;
		h = scale = 0.0;
		if (l > 1) {
			for (k = 1; k <= l; k++) {
				scale += (double)fabs(a[i][k]);
			}
			if (scale == 0.0) {
				//Skip transformation.
				e[i]=a[i][l];
			} else {
				for (k = 1; k <= l; k++) {
					a[i][k] /= scale; //Use scaled a¡Çs for transformation Form ¦Ò in h..
					h += a[i][k] * a[i][k]; 
				}
				f = a[i][l];
				g = (f >= 0.0 ? -(double)sqrt(h) : (double)sqrt(h));
				e[i] = scale * g;
				h -= f * g; //Now h is equation (11.2.4).
				a[i][l] = f - g; //Store u in the ith row of a.
				f = 0.0;
				for (j = 1; j <= l; j++) {
					/* Next statement can be omitted if eigenvectors not wanted */
					a[j][i] = a[i][j] / h; //Store u/H in ith column of a.
					g = 0.0; //Form an element of A ¡¦ u in g.
					for (k = 1; k <= j; k++) {
						g += a[j][k]*a[i][k];
					}
					for (k = j + 1; k <= l; k++) {
						g += a[k][j] * a[i][k];
					}
					e[j] = g / h;
					f += e[j] * a[i][j];
				}
				hh = f / (h + h); //Form K, equation (11.2.11).
				for (j = 1; j <= l; j++) {
					//Form q and store in e overwriting p.
					f = a[i][j];
					e[j] = g = e[j] - hh * f;
					for (k = 1; k <= j; k++) {
						//Reduce a, equation (11.2.13).
						a[j][k] -= (f * e[k] + g * a[i][k]);
					}
				}
			}
		} else {
			e[i] = a[i][l];
		}
		d[i] = h;
	}
	/* Next statement can be omitted if eigenvectors not wanted */
	d[1] = 0.0;
	e[1] = 0.0;
	/* Contents of this loop can be omitted if eigenvectors not
	wanted except for statement d[i]=a[i][i]; */
	for (i = 1; i <= n; i++) {
		//Begin accumulation of transformation matrices.
		l =	i - 1;
		if (d[i]) { 
			//This block skipped when i=1.
			for (j = 1; j <= l; j++) {
				g = 0.0;
				for (k = 1; k <= l; k++) {
					//Use u and u/H stored in a to form P¡¦Q.
					g += a[i][k] * a[k][j];
				}
				for (k = 1;k <= l; k++) {
					a[k][j] -= g*a[k][i];
				}
			}
		}
		d[i] = a[i][i]; //This statement remains.
		a[i][i] = 1.0;  //Reset row and column of a to identity
		for (j = 1; j <= l; j++) {
			a[j][i] = a[i][j] = 0.0; //matrix for next iteration.
		}
	}
}

// QL algorithm with implicit shifts, to determine the eigenvalues and eigenvectors of a real, symmetric,
// tridiagonal matrix, or of a real, symmetric matrix previously reduced by tred2 ¢Û11.2. On
// input, d[1..n] contains the diagonal elements of the tridiagonal matrix. On output, it returns
// the eigenvalues. The vector e[1..n] inputs the subdiagonal elements of the tridiagonal matrix,
// with e[1] arbitrary. On output e is destroyed. When finding only the eigenvalues, several lines
// may be omitted, as noted in the comments. If the eigenvectors of a tridiagonal matrix are desired,
// the matrix z[1..n][1..n] is input as the identity matrix. If the eigenvectors of a matrix
// that has been reduced by tred2 are required, then z is input as the matrix output by tred2.
// In either case, the kth column of z returns the normalized eigenvector corresponding to d[k].

void tqli(double d[], double e[], int n, double **z)
{	
	int m, l, iter, i, j, k;
	double s, r, p, g, f, dd, c, b;
	for (i = 2; i <= n; i++) {
		e[i - 1] = e[i]; //Convenient to renumber the elements of e.
	}
	e[n] = 0.0; 
	for (l = 1; l <= n; l++) {
		iter = 0;
		do {
			for (m = l; m <= n - 1; m++) {
				//Look for a single small subdiagonal element to split the matrix.
				dd = (double)(fabs(d[m]) + fabs(d[m + 1]));
				if ((double)(fabs(e[m]) + dd) == dd) {
					break;
				}
			}
			if (m != l) {
				if (iter++ == 30) {
					nrerror("Too many iterations in tqli");
				}
				g = (double)((d[l + 1] - d[l]) / (2.0 * e[l])); //Form shift.
				r = pythag(g, 1.0);
				g = (double)(d[m] - d[l] + e[l] / (g + SIGN(r, g))); //This is dm . ks.
				s = c = 1.0;
				p = 0.0;
				for (i = m - 1; i >= l; i--) {
					//A plane rotation as in the original QL, followed by Givens rotations to restore tridiagonal form.
					f = s * e[i];
					b = c * e[i];
					e[i + 1] = (r = pythag(f, g));
					if (r == 0.0) { 
						//Recover from underflow.
						d[i + 1] -= p;
						e[m] = 0.0;
						break;
					}
					s = f / r;
					c = g / r;
					g = d[i + 1] - p;
					r = (double)((d[i] - g) * s + 2.0 * c * b);
					d[i + 1] = g + (p = s * r);
					g = c * r - b;
					/* Next loop can be omitted if eigenvectors not wanted*/
					for (k = 1; k <= n; k++) {
						//Form eigenvectors.
						f = z[k][i + 1];
						z[k][i + 1] = s * z[k][i] + c * f;
						z[k][i] = c * z[k][i] - s * f;
					}
				}
				if (r == 0.0 && i >= l) {
					continue;
				}
				d[l] -= p;
				e[l] = g;
				e[m] = 0.0;
			}
		} while (m != l);
	}
	for (i = 1; i <= n; i++) {
		s = 0;
		for (j = 1; j <= n; j++) {
			s += z[i][j] * z[i][j];
		}
		s = (double)sqrt(s);
		for (j = 1; j <= n; j++) {
			z[i][j] /= s;
		}
	}
}

void Eig_Tridiagonal(double **a, int n, double d[])
{		
	double *e;
	e = dvector(1, n);
	tred2(a, n, d, e);
	tqli(d, e, n, a);
	eigsrt(d, a, n);
	free_dvector(e, 1, n);
}

double **Mat_Mul(double  **a, double  **b, int n, int m, int l) 
{
	int     i, j, k;
	double   s;
	double **mat;
	mat = dmatrix(1, n, 1, l);
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= l; j++) {
			s = 0;
			for (k = 1; k <= m; k++) {
				s += a[i][k] * b[k][j];
			}
			mat[i][j] = s;
		}
	}
	return mat;
}

void SelfMat_SclarMul(double **a, double b, int m, int n) 
{
	int i, j;	
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] *= b;
		}
	}
}

void Mat_Minus(double **a, double **b, int m, int n, double **c)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			c[i][j] = a[i][j] - b[i][j];
		}
	}		
}

void SelfMat_Minus(double **a, double **b, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] = a[i][j] - b[i][j];
		}
	}		
}

void Mat_Plus(double **a, double **b, int m, int n, double **c)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			c[i][j] = a[i][j] + b[i][j];
		}
	}		
}

void SelfMat_Plus(double **a, double **b, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] = a[i][j] + b[i][j];
		}
	}		
}

void Mat_Mul(double **a, float **b, int n, int m, int l, double **mat) 
{
	int i, j, k;
	double s;	
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= l; j++) {
			s = 0;
			for (k = 1; k <= m; k++) {
				s += a[i][k] * b[k][j];
			}
			mat[i][j] = s;
		}
	}
}

void Mat_Mul(double **a, double **b, int n, int m, int l, double **mat) 
{
	int i, j, k;
	double s;	
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= l; j++) {
			s = 0;
			for (k = 1; k <= m; k++) {
				s += a[i][k] * b[k][j];
			}
			mat[i][j] = s;
		}
	}
}

double  **Mat_Tranpose(double **a, int n, int m) 
{
	int i, j;
	double **mat;
	mat = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = a[j][i];
		}
	}
	return mat;
}

void Mat_Tranpose(double **a, int n, int m, double **mat) 
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = a[j][i];
		}
	}	
}

void Mat_Tranpose(float **a, int n) 
{
	int i, j;
	float **mat;
	mat = matrix(1, n, 1, n);
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = a[j][i];
		}
	}
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] = mat[i][j];
		}
	}
	free_matrix(mat, 1, n, 1, n);
}

void Mat_Tranpose(double **a, int n) 
{
	int i, j;
	double **mat;
	mat = dmatrix(1, n, 1, n);
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = a[j][i];
		}
	}
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			a[i][j] = mat[i][j];
		}
	}
	free_dmatrix(mat, 1, n, 1, n);
}

void Mat_Mul_diag(double **a, double *b, int n, int m) 
{
	int i, j;	
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= m; j++) {
			a[i][j] *= b[j];
		}
	}
}

void Mat_Mul(double **a, double *b, int n, int m, double *c) 
{
	int i, j;
	double s;	
	for (i = 1; i <= n; i++) {
		s = 0;
		for (j = 1; j <= m; j++) {
			s += a[i][j] * b[j];
		}
		c[i] = s;
	}	
}

double *Mat_Mul(double **a, double *b, int n, int m) 
{
	int i, j;
	double *res, s;
	res = dvector(1, n);
	for (i = 1; i <= n; i++) {
		s = 0;
		for (j = 1; j <= m; j++) {
			s += a[i][j] * b[j];
		}
		res[i] = s;
	}
	return res;
}


void Mat_Mul(double *a, double **b, int n, int m, double *c)
{
	int i, j;
	double s;	
	for (j = 1; j <= m; j++) {
		s = 0;
		for (i = 1; i <= n; i++) {
			s += a[i] * b[i][j];
		}
		c[j] = s;
	}	
}

double *Mat_Mul(double *a, double **b, int n, int m)
{
	int i, j;
	double *res, s;
	res = dvector(1, m);
	for (j = 1; j <= m; j++) {
		s = 0;
		for (i = 1; i <= n; i++) {
			s += a[i] * b[i][j];
		}
		res[j] = s;
	}
	return res;
}

void TranseMatMul(double **a, double **b, int n, int m, int l, double **c) 
{
	double **temp;
	temp = Mat_Tranpose(a, m, n);
	Mat_Mul(temp, b, n, m, l, c);
	free_dmatrix(temp, 1, n, 1, m);
}

void MatMulTranse(double **a, double **b, int n, int m, int l, double **c) 
{
	double **temp;
	temp = Mat_Tranpose(b, l, m);
	Mat_Mul(a, temp, n, m, l, c);
	free_dmatrix(temp, 1, m, 1, l);
}

////////ËË °ÒË§²÷ ÃÔµÛËË ´Ý¾À¼³°ª¼³¸ÒÃÔµÛËËµ¹°Ö °¡¼³ÂÙ³Þ.
BOOL Mat_sqrt(double **mat, int n, double **sqrtmat)
{
	int i;
	double *d;
	double **Transe;	
	d = dvector(1, n);
	Eig_Tridiagonal(mat, n, d);
	Transe = Mat_Tranpose(mat, n, n);
	for (i = 1; i <= n; i++) {
		if (d[i] < 0) {
			return FALSE;
		}
		d[i] = (double)sqrt(d[i]);
	}
	Mat_Mul_diag(mat, d, n, n);
	Mat_Mul(mat, Transe, n, n, n, sqrtmat);

	free_dmatrix(Transe, 1, n, 1, n);
	free_dvector(d, 1, n);
	return TRUE;
}

BOOL Mat_sqrt(double **mat, int n)
{
	int i, j;
	double *d;
	double **Transe;
	double **sqrtmat;
	d = dvector(1, n);
	Eig_Tridiagonal(mat, n, d);
	Transe = Mat_Tranpose(mat, n, n);
	for (i = 1; i <= n; i++) {
		if (d[i] < 0) {
			return FALSE;
		}
		d[i] = (double)sqrt(d[i]);
	}
	Mat_Mul_diag(mat, d, n, n);
	sqrtmat = Mat_Mul(mat, Transe, n, n, n);
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = sqrtmat[i][j];
		}
	}
	free_dmatrix(Transe, 1, n, 1, n);
	free_dmatrix(sqrtmat, 1, n, 1, n);
	free_dvector(d, 1, n);
	return TRUE;
}

void Ortho(double **A, int m, int n, double **Q)
{
	double **v;
	double *w;
	int i, j, rank;
	double s;
	v = dmatrix(1, n, 1, n);
	w = dvector(1, n);
	svdcmp(A, m, n, w, v);
	s = 0;
	rank = 0;
	for (i = 1; i <= n; i++) {
		if (w[i] > 0.00001) {
			rank++;
		}
	}
	for (i = 1; i <= m; i++)	{
		for (j = 1; j <= rank; j++) {
			Q[i][j] = A[i][j];
		}
	}
	free_dmatrix(v, 1, n, 1, n);
	free_dvector(w, 1, n);
}

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

// "Minimal¡É random number generator of Park and Miller with Bays-Durham shuffle and added
// safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint values).
// Call with idum a negative integer to initialize; thereafter, do not alter idum between
// successive deviates in a sequence. RNMX should approximate the largest doubleing value that is less than 1.

double ran1(long *idum)
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];
	double temp;
	if (*idum <= 0 || !iy) { 
		//Initialize.
		if (-(*idum) < 1) {
			*idum = 1; //Be sure to prevent idum = 0.
		}else {
			*idum = -(*idum);
		}
		for (j = NTAB + 7; j >= 0; j--) { 
			//Load the shuffle table (after 8 warm-ups).
			k = (*idum) / IQ;
			*idum = IA * (*idum - k * IQ) - IR * k;
			if (*idum < 0) {
				*idum += IM;
			}
			if (j < NTAB) {
				iv[j] = *idum;
			}
		}
		iy = iv[0];
	}
	k = (*idum) / IQ; //Start here when not initializing.
	*idum = IA * (*idum - k * IQ) - IR * k; //Compute idum=(IA*idum) % IM without over flows by Schrage¡Çs method.
	if(*idum < 0) {
		*idum += IM; 
	}
	j = iy / NDIV; 
	iy = iv[j];
	iv[j] = *idum; 
	if ((temp = (double)(AM * iy)) > RNMX) {
		return (double)RNMX; 
	} else {
		return temp;
	}
}

// Returns a normally distributed deviate with zero mean and unit variance, using ran1(idum)
// as the source of uniform deviates.

double gasdev(long *idum)
{	
	static int iset = 0;
	static double gset;
	double fac, rsq, v1, v2;
	if (*idum < 0) {
		iset = 0; 
	}
	if (iset == 0) { 
		do {
			v1 = (double)(2.0 * ran1(idum) - 1.0); //pick two uniform numbers in the square 
			v2 = (double)(2.0 * ran1(idum) - 1.0);
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0); 
		fac = (double)(sqrt(-2.0 * log(rsq) / rsq));
		gset = v1 * fac;
		iset = 1; 
		return v2 * fac;
	} else {
		iset = 0; 
		return gset; 
	}
}

double **Randn(int m, int n)
{
	double **mat;
	int i, j;
	long p;
	srand( (unsigned)time( NULL ) );

	mat = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <=n; j++)	{
			p = rand();
			mat[i][j] = gasdev(&p);
		}
	}

	return mat;
}

void Randn(int m, int n, double **mat)
{	
	int i, j;
	long p;
	srand( (unsigned)time( NULL ) );	
	mat = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <=n; j++)	{
			p = rand();
			mat[i][j] = gasdev(&p);
		}
	}	
}

double **Rand(int m, int n)
{
	double **mat;
	int i, j;	
	srand( (unsigned)time( NULL ) );
	
	mat = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <=n; j++)	{
			mat[i][j] = (double)rand() / (RAND_MAX + 1);
		}
	}	
	return mat;
}

void MatInitial(double **mat, int m, int n, double val)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = val;
		}
	}
}

void MatCopy(double **mat, double **temp, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = temp[i][j];
		}
	}
}

void MatCopy(double **mat, float **temp, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = temp[i][j];
		}
	}
}

void SymmOrtho(double **mat, int m, int n)
{
	double **temp, **temp1;
	temp = Mat_Tranpose(mat, m, n);
	temp1 = Mat_Mul(temp, mat, n, m, n);
	free_dmatrix(temp, 1, n, 1, m);

	InvMatrix(temp1, n);
	Mat_sqrt(temp1, n);
	temp = Mat_Mul(mat, temp1, m, n, n);
	free_dmatrix(temp1, 1, n, 1, n);

	MatCopy(mat, temp, m, n);
	free_dmatrix(temp, 1, n, 1, m);
}

double FindMinAbsOfDiagMat(double **mat, int n)
{
	int i;
	double min;
	min = (double)INT_MAX;
	for (i = 1; i <= n; i++) {
		if (fabs(mat[i][i]) < min) {
			min = (double)fabs(mat[i][i]);
		}
	}
	return min;
}

double Mat_Norm(double **A, int m, int n)
{
	int i, j;
	double s;
	s = 0;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			s += A[i][j] * A[i][j];
		}
	}
	s = (double)sqrt(s);
	return s;
}

void SelfMat_Pow(double **mat, int m, int n, double alpha)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = (double)pow(mat[i][j], alpha);
		}
	}	
}

double **Mat_Pow(double **mat, int m, int n, double alpha)
{
	int i, j;
	double **Pow;
	Pow = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			Pow[i][j] = (double)pow(mat[i][j], alpha);
		}
	}
	return Pow;
}

double *MatSum(double **mat, int m, int n)
{
	int i, j;
	double *sum, s;
	sum = dvector(1, n);
	for (j = 1; j <= n; j++) {
		s = 0;
		for (i = 1; i <= m; i++) {
			s += mat[i][j];
		}
		sum[j] = s;
	}
	return sum;
}

void SelfPointDivide(double **mat1, double **mat2, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat1[i][j] /= (mat2[i][j] + 0.000001f);
		}
	}
}

double **PointMul(double **mat1, double **mat2, int m, int n)
{
	int i, j;
	double **mat;
	mat = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = mat1[i][j] * mat2[i][j];
		}
	}
	return mat;
}

void SelfPointMul(double **mat1, double **mat2, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat1[i][j] = mat1[i][j] * mat2[i][j];
		}
	}
}

double TanhFunc(double x)
{
	double y;
	y = (double)(1 - 2 / (1 + exp(2 * x)));
	return y;
}

void mat_Tanh(double **mat, int m, int n)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = TanhFunc(mat[i][j]);
		}
	}
}

void Selfmat_Gauss(double **mat, int m, int n, double a2)
{
	int i, j;
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			mat[i][j] = (double)exp(-a2 * mat[i][j] * mat[i][j] / 2);
		}
	}
}

double **mat_Gauss(double **mat, int m, int n, double a2)
{
	int i, j;
	double **res;
	res = dmatrix(1, m, 1, n);
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			res[i][j] = (double)exp(-a2 * mat[i][j] * mat[i][j] / 2);
		}
	}
	return res;
}

double **cov(double **X, int m, int n, int flag)
{
	int i, j;
	double *mean, s;
	double **cov_mat;
	double **xc;
	xc = dmatrix(1, m, 1, n);
	
	cov_mat = dmatrix(1, n, 1, n);
	mean = dvector(1, n);
	for (j = 1; j <= n; j++) {
		s = 0;
		for (i = 1; i <= m; i++) {
			s += X[i][j];
		}
		mean[j] = s / m;
	}
	
	for (i = 1; i <= m; i++) {
		for (j = 1; j <= n; j++) {
			xc[i][j] = X[i][j] - mean[j];
		}
	}
	TranseMatMul(xc, xc, n, m, n, cov_mat);
	if (flag == 0) {
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= n; j++) {
				cov_mat[i][j] /= (m - 1);
			}
		}
	}
	if (flag == 1) {
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= n; j++) {
				cov_mat[i][j] /= m;
			}
		}
	}
	free_dvector(mean, 1, n);
	free_dmatrix(xc, 1, m, 1, n);
	return cov_mat;
}

void svbksb(double **u, double w[], double **v, int m, int n, double b[], double x[])
//////Solves AX = B for a vector X, where A is specied by the arrays u[1..m][1..n], w[1..n],
//////v[1..n][1..n] as returned by svdcmp. m and n are the dimensions of a, and will be equal for
//////square matrices. b[1..m] is the input right-hand side. x[1..n] is the output solution vector.
//////No input quantities are destroyed, so the routine may be called sequentially with dierent b's.
{
	int   jj, j, i;
	double s, *tmp;
	tmp = dvector(1, n);
	for (j = 1; j <= n; j++) { //Calculate UTB.
		s = 0.0;
		if (w[j]) { //Nonzero result only if wj is nonzero.
			for (i = 1; i <= m; i++) {
				s += u[i][j] * b[i];
			}
			s /= w[j]; //This is the divide by wj .
		}
		tmp[j] = s;
	}
	for (j = 1; j <= n; j++) { //Matrix multiply by V to get answer.
		s = 0.0;
		for (jj = 1;jj <= n; jj++) {
			s += v[j][jj] * tmp[jj];
		}
		x[j] = s;
	}
	free_dvector(tmp, 1, n);
}

void LinearSolve_SVD(double **a, int m, int n, double b[], double x[])
{
	double  *w, **v;
	v = dmatrix(1, n, 1, n);
	w = dvector(1, n);
	svdcmp(a, m, n, w, v);
	svbksb(a, w, v, m, n, b, x); 

	free_dmatrix(v, 1, n, 1, n);
	free_dvector(w, 1, n);
}


#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
// Expand in storage the covariance matrix covar, so as to take into account parameters that are
// being held fixed. (For the latter, return zero covariances.)
void covsrt(double **covar, int ma, int ia[], int mfit)
{
	int     i, j, k;
	double swap;
	for (i = mfit + 1; i <= ma; i++) {
		for (j = 1; j <= i; j++) {
			covar[i][j] = covar[j][i] = 0.0;
		}
	}
	k = mfit;
	for (j = ma; j >= 1; j--) {
		if (ia[j]) {
			for (i = 1; i <= ma; i++) {
				SWAP(covar[i][k], covar[i][j])
			}
			for (i = 1; i <= ma; i++) {
				SWAP(covar[k][i], covar[j][i])
			}
			k--;
		}
	}
}

// Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. a[1..n][1..n]
// is the input matrix. b[1..n][1..m] is input containing the m right-hand side vectors. On
// output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of solution
// vectors.
BOOL gaussj(double **a, int n, double **b, int m)
{
	int     *indxc, *indxr, *ipiv;
	int     i, icol, irow, j, k, l, ll;
	double big, dum, pivinv;
	double swap;
	indxc = ivector(1, n); // The integer arrays ipiv, indxr, and indxc are used for bookkeeping on the pivoting.
	indxr = ivector(1, n); 
	ipiv=ivector(1, n);
	for (j = 1; j <= n; j++) {
		ipiv[j] = 0;
	}
	for (i = 1; i <= n; i++) { // This is the main loop over the columns to be reduced.
		big = 0.0; 
		for (j = 1; j <= n; j++) {
			if (ipiv[j] != 1) {
				for (k = 1; k <= n; k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					} 
				}
			}
		}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l = 1; l <= n; l++) {
				SWAP(a[irow][l],a[icol][l])
			}
			for (l = 1; l <= m; l++) {
				SWAP(b[irow][l],b[icol][l])
			}
		}
		indxr[i] = irow; 
		indxc[i] = icol;
		if (a[icol][icol] == 0.0) return FALSE; // nrerror("gaussj: Singular Matrix-2");
		pivinv = 1.0f / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 1; l <= n; l++) {
			a[icol][l] *= pivinv;
		}
		for (l = 1; l <= m; l++) {
			b[icol][l] *= pivinv;
		}

		for (ll = 1; ll <= n; ll++) {
			if (ll != icol) {
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 1; l <= n; l++) {
					a[ll][l] -= a[icol][l] * dum;
				}
				for (l = 1; l <= m; l++) {
					b[ll][l] -= b[icol][l] * dum;
				}
			}
		}
	}
	for (l = n; l >= 1; l--) {
		if (indxr[l] != indxc[l]) {
			for (k = 1; k <= n; k++) {
				SWAP(a[k][indxr[l]], a[k][indxc[l]]);
			}
		}
	}
	free_ivector(ipiv, 1, n);
	free_ivector(indxr, 1, n);
	free_ivector(indxc, 1, n);
	return TRUE;
}


// Converts a square matrix a[1..n][1..n] into row - indexed sparse storage mode.Only elements of 
// a with magnitude ≥thresh are retained.Output is in two linear arrays with dimension nmax(an input parameter) : sa[1..] 
// contains array values, indexed by ija[1..].The
// number of elements filled of sa and ija on output are both ija[ija[1] - 1] - 1 (see text).
void sprsin(double **a, int n, double thresh, unsigned long nmax, double sa[], unsigned long ija[])
{
	int i, j;
	unsigned long k;

	for (j = 1;j <= n;j++) {
		sa[j] = a[j][j]; //Store diagonal elements.
	}
	ija[1] = n + 2; //Index to 1st rowoff - diagonal element, if any.
	k = n + 1;
	for (i = 1; i <= n; i++) {	// Loop over rows.
		for (j = 1; j <= n; j++) {  // Loop over columns.
			if (fabs(a[i][j]) >= thresh && i != j) {
				if (++k > nmax) {
					nrerror("sprsin: nmax too small");
				}
				sa[k] = a[i][j]; //Store off - diagonal elements and their columns.
				ija[k] = j;
			}
		}
		ija[i + 1] = k + 1; // As each rowis completed, store index to
	}
}

int cmpfunc(const void * a, const void * b) {
	ItemSort* itemA = (ItemSort*)a;
	ItemSort* itemB = (ItemSort*)b;
	return (itemA->colIndex - itemB->colIndex);
}

void sprsin_FromCordiToRow(double** dataPerRow, ItemSort **colsPerRow, int* numPerRow, int n, double sa[], unsigned long ija[]) 
{
	int nStartIndex = n + 2;
	int nRealIndex;
	int col, prevCol;
	int i, ind;
	for (i = 1; i <= n; i++) {
		nRealIndex = 0;
		prevCol = 0;
		colsPerRow[i][0].colIndex = 0;
		qsort(colsPerRow[i], numPerRow[i] + 1, sizeof(ItemSort), cmpfunc);
		for (int j = 1; j <= numPerRow[i]; j++) {
			col = colsPerRow[i][j].colIndex;
			ind = colsPerRow[i][j].oldIndex;
			if (i != col) {//non diag
				if (col != prevCol) {
					nRealIndex++;
					ija[nStartIndex + nRealIndex - 1] = col;
				}
				sa[nStartIndex + nRealIndex - 1] += dataPerRow[i][ind];
				prevCol = col;
			} else {
				sa[i] += dataPerRow[i][ind];
			}
		}
		ija[i] = nStartIndex;
		nStartIndex += nRealIndex;
	}
	ija[i] = nStartIndex;
}

void sprsin_FromCordiToRow(double *val, int row_inds[], int col_inds[], int nonzero_num, int n, double sa[], unsigned long ija[])
{
	int i, j;
	unsigned long k;

	struct Item {
		int col;
		double value;
	};
	vector<vector<Item>> vectorInd;
	for (int i = 0; i <= n; i++) {
		vector<Item> t;
		Item tItem = { 0, 0 };
		t.push_back(tItem);
		vectorInd.push_back(t);
	}

	memset(sa + 1, 0, (nonzero_num + n) * sizeof(double));
	for (i = 1; i <= nonzero_num; i++) {
		if (row_inds[i] == col_inds[i])	{			
			sa[row_inds[i]] += val[i];
		}
		Item t;
		t.col = col_inds[i];
		t.value = val[i];
		vectorInd.at(row_inds[i]).push_back(t);
	}
 	ija[1] = n + 2; //Index to 1st rowoff - diagonal element, if any.
 	k = n + 1;
 	for (i = 1; i <= n; i++) {	// Loop over rows.		
		for (j = 1; j < (int)vectorInd[i].size(); j++) {  // Loop over columns.
			if (i != vectorInd[i][j].col) {
				k++;
				sa[k] += vectorInd[i][j].value; //Store off - diagonal elements and their columns.
				ija[k] = vectorInd[i][j].col;
			}
		}
		ija[i + 1] = k + 1; // As each rowis completed, store index to
	}
	vectorInd.clear();
}

// Multiply a matrix in row - index sparse storage arrays sa and ija by a vector x[1..n], giving
// a vector b[1..n].
void sprsax(float sa[], unsigned long ija[], float x[], float b[], unsigned long n)
{
	unsigned long i, k;
	if (ija[1] != n + 2) {
		nrerror("sprsax: mismatched vector and matrix");
	}
	for (i = 1; i <= n; i++) {
		b[i] = sa[i] * x[i]; //Start with diagonal term.
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) { //Loop over off - diagonal terms.			
			b[i] += sa[k] * x[ija[k]];
		}
	}
}
#define USING_MMX TRUE
#if USING_MMX == TRUE
//#include "emmintrin.h"
#endif
void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i, k;
	if (ija[1] != n + 2) {
		nrerror("sprsax: mismatched vector and matrix");
	}
#if (USING_MMX == FALSE)
	__m128d bM;
	__m128d saM;
	__m128d xM;
	__m128d tM;
	for (i = 1; i <= n; i++) {
		//b[i] = sa[i] * x[i]; //Start with diagonal term.
		saM = _mm_load_sd(&sa[i]);
		xM = _mm_load_sd(&x[i]);
		bM = _mm_mul_sd(saM, xM);
		for (k = ija[i]; k <= ija[i + 1] - 1; k+=2) { //Loop over off - diagonal terms.
			//b[i] += sa[k] * x[ija[k]];
			saM = _mm_load_pd(&sa[k]);
			xM = _mm_set_pd(x[ija[k+1]], x[ija[k]]);
			tM = _mm_mul_pd(saM, xM);
			bM = _mm_add_pd(bM, tM);
		}
		b[i] = bM.m128d_f64[0] + bM.m128d_f64[1];
	}
#else
	for (i = 1; i <= n; i++) {
		b[i] = sa[i] * x[i]; //Start with diagonal term.
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) { //Loop over off - diagonal terms.
			b[i] += sa[k] * x[ija[k]];
		}
	}
#endif
}

void sprsum_col(double sa[], unsigned long ija[], double x[], unsigned long n)
{
	double s = 0;
	unsigned long i, k;
	
	for (i = 1; i <= n; i++) {
		s = sa[i]; //Start with diagonal term.
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) { //Loop over off - diagonal terms.
			s += sa[k];
		}
		x[i] = s;
	}
}

// Multiply the transpose of a matrix in row - index sparse storage arrays sa and ija by a vector
// x[1..n], giving a vector b[1..n].
void sprstx(float sa[], unsigned long ija[], float x[], float b[], unsigned long n)
{	
	unsigned long i, j, k;
	if (ija[1] != n + 2) {
		nrerror("mismatched vector and matrix in sprstx");
	}
	for (i = 1; i <= n; i++) {
		b[i] = sa[i] * x[i]; //Start with diagonal terms.
	}
	for (i = 1; i <= n; i++) { //Loop over off - diagonal terms.
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) {
			j = ija[k];
			b[j] += sa[k] * x[i];
		}
	}
}

void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i, j, k;
	if (ija[1] != n + 2) {
		nrerror("mismatched vector and matrix in sprstx");
	}
	for (i = 1; i <= n; i++) {
		b[i] = sa[i] * x[i]; //Start with diagonal terms.
	}
	for (i = 1; i <= n; i++) { //Loop over off - diagonal terms.
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) {
			j = ija[k];
			b[j] += sa[k] * x[i];
		}
	}
}

#define EPS1 1.0e-14
unsigned long *g_ija;
double *g_sa;
float *gf_sa;

void atimes(unsigned long n, float x[], float r[], int itrnsp)
{
	if (itrnsp) sprstx(gf_sa, g_ija, x, r, n);
	else sprsax(gf_sa, g_ija, x, r, n);
}

void atimes(unsigned long n, double x[], double r[], int itrnsp)
{
	if (itrnsp) dsprstx(g_sa, g_ija, x, r, n);
	else dsprsax(g_sa, g_ija, x, r, n);
}

void asolve(unsigned long n, double b[], double x[], int itrnsp)
{
	unsigned long i;
	for (i = 1; i <= n; i++) {
		x[i] = (g_sa[i] != 0.0 ? b[i] / g_sa[i] : b[i]);
	}	
}

void asolve(unsigned long n, float b[], float x[], int itrnsp)
{
	unsigned long i;
	for (i = 1; i <= n; i++) {
		x[i] = (g_sa[i] != 0.0f ? b[i] / gf_sa[i] : b[i]);
	}
}


//Compute one of two norms for a vector sx[1..n], as signaled by itol.Used by linbcg.
float snrm(unsigned long n, float sx[], int itol)
{
	unsigned long i, isamax;
	float ans;
	if (itol <= 3) {
		ans = 0.0;
		for (i = 1; i <= n; i++) {
			ans += sx[i] * sx[i]; //Vector magnitude norm.
		}
		return sqrt(ans);
	}
	else {
		isamax = 1;
		for (i = 1;i <= n;i++) {
			//Largest component norm.
			if (fabs(sx[i]) > fabs(sx[isamax])) {
				isamax = i;
			}
		}
		return fabs(sx[isamax]);
	}
}

double snrm(unsigned long n, double sx[], int itol)
{
	unsigned long i, isamax;
	double ans;
	if (itol <= 3) {
		ans = 0.0;
		for (i = 1; i <= n; i++) {
			ans += sx[i] * sx[i]; //Vector magnitude norm.
		}
		return sqrt(ans);
	} else {
		isamax = 1;
		for (i = 1;i <= n;i++) {
			//Largest component norm.
			if (fabs(sx[i]) > fabs(sx[isamax])) {
				isamax = i;
			}
		}
		return fabs(sx[isamax]);
	}
}

// Solves A · x = b for x[1..n], given b[1..n], by the iterative biconjugate gradient method.
// On input x[1..n] should be set to an initial guess of the solution(or all zeros); itol is 1, 2, 3, or 4, 
// specifying which convergence test is applied(see text); itmax is the maximum number
// of allowed iterations; and tol is the desired convergence tolerance.On output, x[1..n] is
// reset to the improved solution, iter is the number of iterations actually taken, and err is the
// estimated error.The matrix A is referenced only through the user - supplied routines atimes,
// which computes the product of either A or its transpose on a vector; and asolve, which solves
// A · x = b or AT · x = b for some preconditioner matrix A (possibly the trivial diagonal part of A).
void linbcg(unsigned long n, double b[], double x[], int itol, double tol, int itmax, int *iter, double *err)
{
	unsigned long j;
	double ak, akden, bk, bkden, bknum, bnrm, dxnrm, xnrm, zm1nrm, znrm;
	double *p, *pp, *r, *rr, *z, *zz;
	p  = dvector(1, n);
	pp = dvector(1, n);
	r  = dvector(1, n);
	rr = dvector(1, n);
	z  = dvector(1, n);
	zz = dvector(1, n);
	*iter = 0;
	atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
		rr[j] = r[j];
	}
	/* atimes(n,r,rr,0); */ //Uncomment this line to get the “minimum residual” variant of the algorithm.
	if(itol == 1) {
		bnrm = snrm(n, b, itol);
		asolve(n, r, z, 0); //Input to asolve is r[1..n], output is z[1..n]; the final 0 indicates that the matrix A (not its transpose) is to be used.
	} else if (itol == 2) {
		asolve(n, b, z, 0);
		bnrm = snrm(n, z, itol);
		asolve(n, r, z, 0);
	} else if (itol == 3 || itol == 4) {
		asolve(n, b, z, 0);
		bnrm = snrm(n, z, itol);
		asolve(n, r, z, 0);
		znrm = snrm(n, z, itol);
	} else {
		nrerror("illegal itol in linbcg");
	}
	while (*iter <= itmax) {//	Main loop.
		++(*iter);
		asolve(n, rr, zz, 1); //Final 1 indicates use of transpose matrix AT .
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += z[j] * rr[j];
		}
		//Calculate coefficient bk and direction vectors p and pp.
		if (*iter == 1) {
			for (j = 1;j <= n;j++) {
				p[j] = z[j];
				pp[j] = zz[j];
			}
		} else {
			bk = bknum / bkden;
			for (j = 1;j <= n;j++) {
				p[j] = bk*p[j] + z[j];
				pp[j] = bk*pp[j] + zz[j];
			}
		}
		bkden = bknum; //Calculate coefficient ak, newiterate x, and new residuals r and rr.
		atimes(n, p, z, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += z[j] * pp[j];
		}
		ak = bknum / akden;
		atimes(n, pp, zz, 1);
		for (j = 1;j <= n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*z[j];
			rr[j] -= ak*zz[j];
		}
		asolve(n, r, z, 0); //Solve A · z = r and check stopping criterion.
		if (itol == 1) {
			*err = snrm(n, r, itol) / bnrm;
		} else if (itol == 2) {
			*err = snrm(n, z, itol) / bnrm;
		} else if (itol == 3 || itol == 4) {
			zm1nrm = znrm;
			znrm = snrm(n, z, itol);
			if (fabs(zm1nrm - znrm) > EPS1 * znrm) {
				dxnrm = fabs(ak)*snrm(n, p, itol);
				*err = znrm / fabs(zm1nrm - znrm)*dxnrm;
			} else {
				*err = znrm / bnrm; //Error may not be accurate, so loop again.
				continue;
			}
			xnrm = snrm(n, x, itol);
			if (*err <= 0.5*xnrm) {
				*err /= xnrm;
			} else {
				*err = znrm / bnrm; //Error may not be accurate, so loop again.
				continue;
			}
		}
		printf("iter=%4d err=%12.6f\n", *iter, *err);
// 		if (*err <= tol) {
// 			break;
// 		}		
	}

	dsprsax(g_sa, g_ija, x, p, n);

	float sum = 0;
	for (unsigned long i = 1; i <= n; i++) {
		sum += (float)fabs(p[i] - b[i]);
		if (fabs(p[i] - b[i]) > 0.1) {
			sum = sum;
		}
	}

	free_dvector(p, 1, n);
	free_dvector(pp, 1, n);
	free_dvector(r, 1, n);
	free_dvector(rr, 1, n);
	free_dvector(z, 1, n);
	free_dvector(zz, 1, n);	
}

void linbcg(unsigned long n, float b[], float x[], int itol, float tol, int itmax, int *iter, float *err)
{
	unsigned long j;
	float ak, akden, bk, bkden, bknum, bnrm, dxnrm, xnrm, zm1nrm, znrm;
	float *p, *pp, *r, *rr, *z, *zz;
	p = fvector(1, n);
	pp = fvector(1, n);
	r = fvector(1, n);
	rr = fvector(1, n);
	z = fvector(1, n);
	zz = fvector(1, n);
	*iter = 0;
	atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
		rr[j] = r[j];
	}
	/* atimes(n,r,rr,0); */ //Uncomment this line to get the “minimum residual” variant of the algorithm.
	if (itol == 1) {
		bnrm = snrm(n, b, itol);
		asolve(n, r, z, 0); //Input to asolve is r[1..n], output is z[1..n]; the final 0 indicates that the matrix A (not its transpose) is to be used.
	}
	else if (itol == 2) {
		asolve(n, b, z, 0);
		bnrm = snrm(n, z, itol);
		asolve(n, r, z, 0);
	}
	else if (itol == 3 || itol == 4) {
		asolve(n, b, z, 0);
		bnrm = snrm(n, z, itol);
		asolve(n, r, z, 0);
		znrm = snrm(n, z, itol);
	}
	else {
		nrerror("illegal itol in linbcg");
	}
	while (*iter <= itmax) {//	Main loop.
		++(*iter);
		asolve(n, rr, zz, 1); //Final 1 indicates use of transpose matrix AT .
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += z[j] * rr[j];
		}
		//Calculate coefficient bk and direction vectors p and pp.
		if (*iter == 1) {
			for (j = 1;j <= n;j++) {
				p[j] = z[j];
				pp[j] = zz[j];
			}
		}
		else {
			bk = bknum / bkden;
			for (j = 1;j <= n;j++) {
				p[j] = bk*p[j] + z[j];
				pp[j] = bk*pp[j] + zz[j];
			}
		}
		bkden = bknum; //Calculate coefficient ak, newiterate x, and new residuals r and rr.
		atimes(n, p, z, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += z[j] * pp[j];
		}
		ak = bknum / akden;
		atimes(n, pp, zz, 1);
		for (j = 1;j <= n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*z[j];
			rr[j] -= ak*zz[j];
		}
		asolve(n, r, z, 0); //Solve A · z = r and check stopping criterion.
		if (itol == 1) {
			*err = snrm(n, r, itol) / bnrm;
		}
		else if (itol == 2) {
			*err = snrm(n, z, itol) / bnrm;
		}
		else if (itol == 3 || itol == 4) {
			zm1nrm = znrm;
			znrm = snrm(n, z, itol);
			if (fabs(zm1nrm - znrm) > EPS1 * znrm) {
				dxnrm = fabs(ak)*snrm(n, p, itol);
				*err = znrm / fabs(zm1nrm - znrm)*dxnrm;
			}
			else {
				*err = znrm / bnrm; //Error may not be accurate, so loop again.
				continue;
			}
			xnrm = snrm(n, x, itol);
			if (*err <= 0.5*xnrm) {
				*err /= xnrm;
			}
			else {
				*err = znrm / bnrm; //Error may not be accurate, so loop again.
				continue;
			}
		}
		printf("iter=%4d err=%12.6f\n", *iter, *err);
		// 		if (*err <= tol) {
		// 			break;
		// 		}		
	}

	sprsax(gf_sa, g_ija, x, p, n);

	float sum = 0;
	for (unsigned long i = 1; i <= n; i++) {
		sum += fabs(p[i] - b[i]);
		if (fabs(p[i] - b[i]) > 0.1) {
			sum = sum;
		}
	}

	free_vector(p, 1, n);
	free_vector(pp, 1, n);
	free_vector(r, 1, n);
	free_vector(rr, 1, n);
	free_vector(z, 1, n);
	free_vector(zz, 1, n);
}

void ConjugateGradient_DiagPreco(unsigned long n, double b[], double x[], double tol, int itmax, int *iter, double *err)
{
	unsigned long j;
	double ak, akden, beta = 0, bknum;
	double *p, *r, *z, *ap;

	p = dvector(1, n);
	r = dvector(1, n);
	z = dvector(1, n);
	ap = dvector(1, n);
	memcpy(p + 1, z + 1, n * sizeof(double));

	*iter = 0;
	atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
	}
	asolve(n, r, z, 0);
	memcpy(p + 1, z + 1, n * sizeof(double));
	while (*iter <= itmax) {//	Main loop.
		++(*iter);
		*err = snrm(n, r, 1);
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += z[j] * r[j];
		}		
		atimes(n, p, ap, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += ap[j] * p[j];
		}
		ak = bknum / akden;		
		for (j = 1;j <= n;j++) {
 			x[j] += ak*p[j];
 			r[j] -= ak*ap[j];
		}
		asolve(n, r, z, 0); //Solve A · z = r and check stopping criterion.
		for (beta = 0.0, j = 1; j <= n; j++) {
			beta += z[j] * r[j];
		}
		beta /= bknum;
		for (j = 1;j <= n;j++) {
			p[j] = z[j] + beta * p[j];		
		}		
		if (*err <= tol) {
			break;
		}
	}
	dsprsax(g_sa, g_ija, x, p, n);

	float sum = 0;
	for (unsigned long i = 1; i <= n; i++) {
		sum += (float)fabs(p[i] - b[i]);
		if (fabs(p[i] - b[i]) > 0.1) {
			sum = sum;
		}
	}
	free_dvector(p, 1, n);	
	free_dvector(r, 1, n);
	free_dvector(z, 1, n);
	free_dvector(ap, 1, n);
}

#include <ctime>
void ConjugateGradient_DiagPreco(unsigned long n, float b[], float x[], float tol, int itmax, int *iter, float *err)
{
	unsigned long j;
	float ak, akden, bknum, beta = 0;
	float *p, *r, *z, *ap;

	p  = fvector(1, n);
	r  = fvector(1, n);
	z  = fvector(1, n);
	ap = fvector(1, n);
	memcpy(p + 1, z + 1, n * sizeof(float));

	*iter = 0;
	atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
	}
	asolve(n, r, z, 0);
	memcpy(p + 1, z + 1, n * sizeof(float));
	while (*iter <= itmax) {//	Main loop.
		++(*iter);
		*err = snrm(n, r, 1);
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += z[j] * r[j];
		}
		atimes(n, p, ap, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += ap[j] * p[j];
		}
		ak = bknum / akden;
		for (j = 1;j <= n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*ap[j];
		}
		asolve(n, r, z, 0); //Solve A · z = r and check stopping criterion.
		for (beta = 0.0, j = 1; j <= n; j++) {
			beta += z[j] * r[j];
		}
		beta /= bknum;
		for (j = 1;j <= n;j++) {
			p[j] = z[j] + beta * p[j];
		}
		if (*err <= tol) {
			break;
		}
	}
	free_vector(p, 1, n);
	free_vector(r, 1, n);
	free_vector(z, 1, n);
	free_vector(ap, 1, n);
}

void ConjugateGradient(unsigned long n, float b[], float x[], float tol, int itmax, int *iter, float *err)
{	
	unsigned long j;
	float ak, akden, bknum, beta = 0;
	float *p, *r, *z, *ap;

	p = fvector(1, n);
	r = fvector(1, n);
	z = fvector(1, n);
	ap = fvector(1, n);
	memcpy(p + 1, z + 1, n * sizeof(float));

	*iter = 0;
	atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
	}
	memcpy(z + 1, r + 1, n * sizeof(float));
	memcpy(p + 1, z + 1, n * sizeof(float));
	while (*iter <= itmax) {//	Main loop.
		++(*iter);
		*err = snrm(n, r, 1);
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += z[j] * r[j];
		}
		atimes(n, p, ap, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += ap[j] * p[j];
		}
		ak = bknum / akden;
		for (j = 1;j <= n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*ap[j];
		}
		memcpy(z + 1, r + 1, n * sizeof(float));
		for (beta = 0.0, j = 1; j <= n; j++) {
			beta += z[j] * r[j];
		}
		beta /= bknum;
		for (j = 1;j <= n;j++) {
			p[j] = z[j] + beta * p[j];
		}
		if (*err <= tol) {
			break;
		}
	}
	sprsax(gf_sa, g_ija, x, p, n);

	double sum = 0;
	for (unsigned long i = 1; i <= n; i++) {
		sum += fabs(p[i] - b[i]);
		if (fabs(p[i] - b[i]) > 0.1) {
			sum = sum;
		}
	}
	free_vector(p, 1, n);
	free_vector(r, 1, n);
	free_vector(z, 1, n);
	free_vector(ap, 1, n);
}

void SteepestGradient(unsigned long n, double b[], double x[], double tol, int itmax, int *iter, double *err)
{
	unsigned long j;
	double ak, akden, bknum, beta = 0;
	double *p, *r, *z, *ap, gama = 0.00005;

	p = dvector(1, n);
	r = dvector(1, n);
	z = dvector(1, n);
	ap = dvector(1, n);

	double *delta = dvector(1, n);
	memset(delta + 1, 0, n * sizeof(double));
	*iter = 0;		
	memcpy(p + 1, z + 1, n * sizeof(double));
	while (*iter <= itmax) {//	Main loop.
		++(*iter);	

		atimes(n, x, r, 0); //Input to atimes is x[1..n], output is r[1..n]; the final 0 indicates that the matrix(not its transpose) is to be used.
		for (j = 1; j <= n; j++) {
			r[j] = r[j] - b[j];
		}		
		for (bknum = 0.0, j = 1; j <= n; j++) {
			bknum += r[j] * r[j];
		}
		*err = sqrt(bknum);
		atimes(n, r, ap, 0);
		for (akden = 0.0, j = 1; j <= n; j++) {
			akden += ap[j] * r[j];
		}
		ak = bknum / akden;
		for (j = 1;j <= n;j++) {
//			x[j] -= ak * r[j];
 			delta[j] = 0.9 * delta[j] - ak * r[j];
 			x[j] += delta[j];
		}		
		if (*err <= tol) {
			break;
		}
	}
	dsprsax(g_sa, g_ija, x, p, n);

	double sum = 0;
	for (unsigned long i = 1; i <= n; i++) {
		sum += fabs(p[i] - b[i]);
		if (fabs(p[i] - b[i]) > 0.1) {
			sum = sum;
		}
	}
	free_dvector(p, 1, n);
	free_dvector(r, 1, n);
	free_dvector(z, 1, n);
	free_dvector(ap, 1, n);
	free_dvector(delta, 1, n);
}

void ICFactor(long *pntr, long *indx, double *val, int n)
{
	int d, g, h, i, j, k;
	double z;
	
	for (k = 0; k < n - 1; k++) {	
		d = pntr[k];
		z = val[d] = sqrt(val[d]);

		for (i = d + 1; i < pntr[k + 1]; i++)
			val[i] /= z;

		for (i = d + 1; i < pntr[k + 1]; i++) {
			z = val[i];
			h = indx[i];
			g = i;
			for (j = pntr[h]; j < pntr[h + 1]; j++) {
				for (; g < pntr[k + 1] && indx[g + 1] <= indx[j]; g++) {
					if (indx[g] == indx[j]) {
						val[j] -= z * val[g];
					}
				}
			}
		}
	}
 	d = pntr[n - 1];
 	val[d] = sqrt(val[d]);
}

void ICPreconditioner(double **mat, int n, double **Colesky)
{
	int i, j, k;

	for (k = 1; k <= n; k++) {
		mat[k][k] = sqrt(mat[k][k]);
		for (i = k + 1; i <= n; i++) {
 			if (mat[i][k] == 0) {
 				continue;
 			}
			mat[i][k] = mat[i][k] / mat[k][k];
		}
		for (j = k + 1; j <= n; j++) {
			for (i = j; i <= n; i++) {
				if (mat[i][j] == 0) {
					continue;
				}
				mat[i][j] = mat[i][j] - mat[i][k] * mat[j][k];
			}
		}		
	}
}

// void solve_cholsl(double **a, int n, double b[], double x[])
// {
// 	int    i, k;
// 	double sum;
// 	for (i = 1; i <= n; i++) {// Solve L · y = b, storing y in x.
// 		sum = b[i];
// 		for (k = i - 1; k >= 1; k--) {
// 			sum -= a[i][k] * x[k];
// 		}
// 		x[i] = sum / a[i][i];
// 	}
// 	for (i = n; i >= 1; i--) {//	Solve LT · x = y.
// 		sum = x[i];
// 		for (k = i + 1; k <= n; k++) {
// 			sum -= a[k][i] * x[k];
// 		}
// 		x[i] = sum / a[i][i];
// 	}
// }

void solve_cholsl(double **a, int n, double b[], double x[])
{
	int    i, k;
	double sum;	

	double *tmp = (double *)malloc(n * sizeof(double));
	for (i = 1; i <= n; i++) {// Solve L · y = b, storing y in x.
		sum = b[i];
		for (k = i - 1; k >= 1; k--) {
			sum -= a[k][i] * x[k];
		}
		x[i] = sum / a[i][i];
	}
	for (i = n; i >= 1; i--) {//	Solve LT · x = y.
		sum = x[i];
		for (k = i + 1; k <= n; k++) {
			sum -= a[i][k] * x[k];
		}
		x[i] = sum / a[i][i];
	}
    if(tmp) {
        free(tmp);
        tmp = NULL;
    }
}

// void solve_cholsl(long *pntr, long *indx, double *val, int n, double b[], double x[])
// {
// 	int    i, k;
// 	double sum;
// 	for (i = 1; i <= n; i++) {// Solve L · y = b, storing y in x.
// 		sum = b[i];
// 		for (k = i - 1; k >= 1; k--) {
// 			sum -= a[i][k] * x[k];
// 		}
// 		x[i] = sum / a[i][i];
// 	}
// 	for (i = n; i >= 1; i--) {//	Solve LT · x = y.
// 		sum = x[i];
// 		for (k = i + 1; k <= n; k++) {
// 			sum -= a[k][i] * x[k];
// 		}
// 		x[i] = sum / a[i][i];
// 	}
// }

void ICPreconditioner(double *sa, unsigned long *ija, int n, long *pntr, long *indx, double *val, int &nz)
{
	unsigned  i, j;
	int k;

	// Copy just triangular part (including diagonal)
	nz = 0;
	pntr[0] = 0;
	for (k = 0; k < n; k++) {
		pntr[k + 1] = pntr[k];		
		if (sa[k + 1] != 0)	{
			i = pntr[k + 1]++;
			val[i]  = sa[k + 1];
			indx[i] = k;
			nz++;
		}
		for (j = ija[k + 1]; j < ija[k + 2]; j++) {
			if (ija[j] > k + 1) {
				i = pntr[k + 1]++;
				val[i] = sa[j];
				indx[i] = ija[j] - 1;
				nz++;
			}
		}
	}
	ICFactor(pntr, indx, val, n);
}

void ICSolve(long *pntr, long *indx, double *val, int n, double *x)
{
	
}
