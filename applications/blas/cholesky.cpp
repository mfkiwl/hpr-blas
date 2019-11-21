/*****************************************************************
*  Inversion of a symmetric matrix by Cholesky decomposition.    *
*  The matrix must be positive definite.                         * 
* -------------------------------------------------------------- *
* REFERENCE:                                                     *
*             From a Java Library Created by Vadim Kutsyy,       *
*             "http://www.kutsyy.com".                           *
* -------------------------------------------------------------- * 
* SAMPLE RUN:                                                    *
*                                                                *
* Inversion of a square real symetric matrix by Cholevsky method *
* (The matrix must positive definite).                           *
*                                                                *
* Size = 4                                                       *
*                                                                *
* Determinant = 432.000000                                       *
*                                                                *
* Matrix A:                                                      *
* 5.000000 -1.000000 -1.000000 -1.000000                         *
* -1.000000 5.000000 -1.000000 -1.000000                         *
* -1.000000 -1.000000 5.000000 -1.000000                         *
* -1.000000 -1.000000 -1.000000 5.000000                         *
*                                                                *
* Matrix Inv(A):                                                 *
* 0.250000 0.083333 0.083333 0.083333                            *
* 0.083333 0.250000 0.083333 0.083333                            *
* 0.083333 0.083333 0.250000 0.083333                            *
* 0.083333 0.083333 0.083333 0.250000                            *
*                                                                *
*                      C++ Release By Jean-Pierre Moreau, Paris. *
*                                (www.jpmoreau.fr)               *
* -------------------------------------------------------------- *
* Release 1.1 : added verification Inv(A) * A = I.               *
*****************************************************************/
#include <hprblas>
//#include <universal/posit/posit>


// In-place Cholesky factorization of an SPD matrix A, generating the factor UT and the diagonal
template<typename Matrix, typename Vector>
bool CholeskyFactorization(Matrix& A, Vector& diagonal) {
	using Scalar = typename Matrix::value_type;
	int N = int(mtl::mat::num_cols(A));

	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			Scalar sum = A[i][j];
			for (int k = i - 1; k >= 0; k--) {
				sum -= A[i][k] * A[j][k];
			}
			if (i == j) {
				if (sum <= 0) {
					std::cerr << "Matrix is not positive definite!\n";
					return false;
				}
				diagonal[i] = sqrt(sum);
			}
			else {
				A[j][i] = sum / diagonal[i];
			}
		}
	}
	return true;
}

// Cholesky factorization of an SPD matrix A, generating the factor UT and the diagonal
template<typename Matrix, typename Vector>
bool CholeskyFactorization(const Matrix& A, Matrix& UT, Vector& diagonal) {
	using Scalar = typename Matrix::value_type;
	int N = int(mtl::mat::num_cols(A));

	UT = A;
	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			Scalar sum = A[i][j];
			for (int k = i - 1; k >= 0; k--) {
				sum -= UT[i][k] * UT[j][k];
			}
			if (i == j) {
				if (sum <= 0) {
					std::cout << "Matrix is not positive definite!\n";
					return false;
				}
				diagonal[i] = sqrt(sum);
			}
			else {
				UT[j][i] = sum / diagonal[i];
			}
		}
	}
	return true;
}

// CholeskyDecomposition takes a Symmetric Positive Definite Matrix A, and returns the lower triangular Cholesky factorized matrix U transpose (UT)
template<typename Matrix>
bool CholeskyDecomposition(const Matrix& A, Matrix& UT) {
	using Scalar = typename Matrix::value_type;
	int N = int(mtl::mat::num_cols(A));
	mtl::vec::dense_vector<Scalar> diagonal(N);

	bool success = CholeskyFactorization(A, UT, diagonal);
	if (!success) return false;

	// complete the UT by nulling out the upper triangular part
	for (int i = 0; i < N; i++) {
		UT[i][i] = diagonal[i];
		for (int j = i + 1; j < N; j++) {
			UT[i][j] = 0;
		}
	}

	return true;
}

// CholeskyDecomposition takes a Symmetric Positive Definite Matrix A, and returns the lower triangular Cholesky factorized matrix U transpose (UT)
template<typename Matrix>
Matrix CholeskyDecomposition(const Matrix& A) {
	using Scalar = typename Matrix::value_type;
	int N = int(mtl::mat::num_cols(A));
	Matrix UT(N, N);
	mtl::vec::dense_vector<Scalar> diagonal(N);

	UT = 0;
	bool success = CholeskyFactorization(A, UT, diagonal);
	if (!success) {
		UT = 0;
		return UT;
	}

	// complete the UT by nulling out the upper triangular part
	for (int i = 0; i < N; i++) {
		UT[i][i] = diagonal[i];
		for (int j = i + 1; j < N; j++) {
			UT[i][j] = 0;
		}
	}
	return UT;
}
 
/* -----------------------------------------------------
         Inverse of Cholesky decomposition.

         input    A     Symmetric positive def. matrix
         output   Linv  inverse of lower decomposed matrix
         uses        choldc1(Matrix,Vector)         
   ----------------------------------------------------- */
template<typename Matrix>
void choldcsl(const Matrix& A, Matrix& Linv) {
	using Scalar = typename Matrix::value_type;
	int N = int(mtl::mat::num_cols(A));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			Linv[i][j] = A[i][j];
		}
	}

	mtl::vec::dense_vector<Scalar> p(N);
	CholeskyFactorization(Linv, p);
	for (int i = 0; i < N; i++) {
		Linv[i][i] = 1 / p[i];
		for (int j = i + 1; j < N; j++) {
			Scalar sum = Scalar(0);
			for (int k = i; k < j; k++) {
				sum -= Linv[j][k] * Linv[k][i];
			}
			Linv[j][i] = sum / p[j];
		}
	}
}

// Return value of the determinant of a symmetric, positive definite matrix via a Cholesky decomposition
template<typename Matrix, typename Scalar>
Scalar DeterminantSPD(const Matrix& A) {	
	int N = int(mtl::mat::num_cols(A));
	Matrix C(N, N);
	CholeskyDecomposition(A, C);
	Scalar d = Scalar(1);
	for (int i = 0; i < N; i++) {
		d *= C[i][i];
	}
	return d * d;
}
 
/* ---------------------------------------------------
        Matrix inverse using Cholesky decomposition

        input	  A  Symmetric positive def. matrix
        output   Ainv  inverse of A
        uses        choldc1(MAT, VEC)
   --------------------------------------------------- */
template<typename Matrix>
void cholsl(const Matrix& A, Matrix& Ainv) {
	choldcsl(A, Ainv);
//	std::cout << "first   Ainv\n" << Ainv << std::endl;

	int N = int(mtl::mat::num_cols(A));
	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			Ainv[i][j] = 0.0;
		}
	}
//	std::cout << "second   Ainv\n" << Ainv << std::endl;

	for (int i = 0; i < N; i++) {
		Ainv[i][i] *= Ainv[i][i];
		for (int k = i + 1; k < N; k++) {
			Ainv[i][i] += Ainv[k][i] * Ainv[k][i];
		}
		for (int j = i + 1; j < N; j++) {
			for (int k = j; k < N; k++) {
				Ainv[i][j] += Ainv[k][i] * Ainv[k][j];
			}
		}
	}
//	std::cout << "third  Ainv\n" << Ainv << std::endl;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < i; j++) {
			Ainv[i][j] = Ainv[j][i];
		}
	}
//	std::cout << "final  Ainv\n" << Ainv << std::endl;

}


// CheckPositiveDefinite returns true if the Matrix A is a Positive Definite matrix
template<typename Matrix>
bool CheckPositiveDefinite(Matrix& A) {
	using Scalar = typename Matrix::value_type;
	assert(mtl::mat::num_rows(A) == mtl::mat::num_cols(A));  // got a be square
	int N = int(mtl::mat::num_rows(A));
    bool result = true;
	for (int i = 0; i < N; i++) {
	    for (int j = i; j < N; j++) {
              Scalar sum = A[i][j];
			  for (int k = i - 1; k >= 0; k--) {
				  sum -= A[i][k] * A[j][k];
			  }
              result = (i == j) && (sum <= 0.0) ? false : result;
	    }
    }
	return result;
}

/* 
* SAMPLE RUN:                                                    *
*                                                                *
* Inversion of a square real symetric matrix by Cholevsky method *
* (The matrix must positive definite).                           *
*                                                                *
* Size = 4                                                       *
*                                                                *
* Determinant = 432.000000                                       *
*                                                                *
* Matrix A:                                                      *
* 5.000000 -1.000000 -1.000000 -1.000000                         *
* -1.000000 5.000000 -1.000000 -1.000000                         *
* -1.000000 -1.000000 5.000000 -1.000000                         *
* -1.000000 -1.000000 -1.000000 5.000000                         *
*                                                                *
* Matrix Inv(A):                                                 *
* 0.250000 0.083333 0.083333 0.083333                            *
* 0.083333 0.250000 0.083333 0.083333                            *
* 0.083333 0.083333 0.250000 0.083333                            *
* 0.083333 0.083333 0.083333 0.250000                            *
*                                                                *
* SetupMatrix can generate the above test matrix with the call   *
* int N = 4;                                                     *
* Matrix A(N,N);                                                 *
* SetupMatrix(A, N)                                              *
*/
template<typename Matrix>
void SetupMatrix(Matrix& A, int bandwidth = 0) {
	int N = int(mtl::mat::num_rows(A));
	A = 0;
	// define lower half of symmetrical matrix
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (i == j) {
				A[i][i] = 5;
			}
			else {
				int diff = (i - j) > 0 ? (i - j) : (j - i);
				if (diff <= bandwidth) {
					A[i][j] = -1;
				}
			}
		}
	}
}
// main program to demonstrate the use of function cholsl()
int main(int argc, char* argv[]) 
try {
	using namespace std;
	using namespace mtl;
	using Scalar = sw::unum::posit<32,2>;
	using Matrix = mtl::mat::dense2D<Scalar>;
	using Vector = mtl::vec::dense_vector<Scalar>;

	cout << " Inversion of a square real symmetric positive definite matrix by Cholesky method\n";
	constexpr int m = 5;
	constexpr int n = 4;
	constexpr unsigned N = m*n;
	cout << "matrix size is " << N << endl;
	Matrix A(N,N), Ainv(N,N), A1(N,N), UT(N,N), C(N,N), Linv(N,N);
	Vector p(N);

	// intended for N = 4
	SetupMatrix(A, N);
	mtl::mat::laplacian_setup(A, m, n);

	// save a copy for the verification phase, as our in-place Cholesky factorization is destructive
	A1 = A;
	p = 0;

	if (!CheckPositiveDefinite(A)) {
		cout << "This matrix is not positive definite !\n";
		return EXIT_FAILURE;
	}
	cout << "Original Matrix:\n" << A << endl;
	Scalar determinant = DeterminantSPD<Matrix, Scalar>(A);
	cout << "Determinant = " << determinant << endl;

	{ // syntax #1
		UT = Scalar(0);
		if (CholeskyDecomposition(A, UT)) {
			cout << "In-place Cholesky\nU^T Matrix:\n" << UT << endl;
		}
		else {
			cerr << "Couldn't factorize matrix\n";
		}
	}

	{ // syntax #2
		UT = CholeskyDecomposition(A);
		cout << "MATLAB Cholesky\nU^T Matrix:\n" << UT << endl;
	}

	Ainv = UT;
	cholsl(A, Ainv);
	cout << "Matrix Inv(A)  :\n" << Ainv << endl;

	cout << "verification of the decomposition U^T * U = A\n";
	C = A1 * Ainv;
	cout << "Verification A * Inv(A) = I:\n" << C << endl;
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_arithmetic_exception& err) {
	std::cerr << "Uncaught posit arithmetic exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const quire_exception& err) {
	std::cerr << "Uncaught quire exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const posit_internal_exception& err) {
	std::cerr << "Uncaught posit internal exception: " << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (std::runtime_error& err) {
	std::cerr << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
