#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <type_traits>
#include "veronica_type.h"

#if defined INTEL_MKL
    #include "mkl.h"
#endif

namespace veronica
{
    template <typename FP> void print_matrix(const FP* matrix, const int row, const int col)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                cout << (double)matrix[i * col + j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    template <typename FP> void init_matrix(FP* matrix, const int row, const int col)
    {
        cout << "[info] init matrix addr" << hex << (uint64)matrix
             << " with row = " << row << ", col = " << col << endl;
        
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                matrix[i * col + j] = (FP)rand() / (FP)RAND_MAX;
                // matrix[i * col + j] = (FP) i+j; // may be helpful for debugging
            }
        }
    }

    template <typename FP> void kij_gemm(const FP* a, const FP* b, FP* result, const int m, const int n, const int p)
    {
        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < m; i++)
            {
                FP r = a[i * n + k];
                for (int j = 0; j < p; j++)
                {
                    result[i * p + j] += r * b[k * p + j];
                }
            }
        }
    }

#if defined(INTEL_MKL)
    void mkl_gemm(const FP* a, const FP* b, FP* result, const int m, const int n, const int p)
    {
        static_if(constexpr (std::is_same<FP, float>::value))
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, p, n, 1.0, a, n, b, p, 0.0, result, p);
        static_if(constexpr (std::is_same<FP, double>::value))
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, p, n, 1.0, a, n, b, p, 0.0, result, p);
    }
#endif

    template <typename FP> void check_matrix(FP* to_verif, FP* correct, const int row, const int col)
    {
        double total = 0;
        double max = 0;

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                double error = abs(to_verif[i * col + j] - correct[i * col + j]);
                total += error;
                if(error > max) max = error;
            }
        }

        cout << "[info] average error is " << total/(row * col)
             << ",  maximum error is " << max << endl;
    }
}