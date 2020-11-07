#include <omp.h>
#include <iterator>
#include <cmath>
#include <fstream>

#include "long_digits_multiplication_searching.h"
#include "stream/stream.h"

namespace lipaboy_lib::numberphile {

    void long_digits_multiplication_searching_long_unsigned_in_any_base()
    {
        #define NUM_THREADS_UNSIGNED 1
        constexpr unsigned int BASE = 3;
        constexpr int64_t MAX_LEN = 120;
        using NumType = LongUnsigned<9>;
        using MaxNumType = //LongUnsigned<1>;
            NumType;

        using OneDigitNumType =
            LongUnsigned<1>;
        using TArray = std::array<NumType, 16>;
        using special::pow;

        using PrimeType = std::array<int, 6>;
        constexpr PrimeType primes = { 2, 3, 5, 7, 11, 13 };

        auto startTime = extra::getCurrentTime();

        TArray nums;
        nums.fill(NumType(1));
        int64_t maxSteps = 0;
        MaxNumType maxNumber(0);

        int counter = 2;
        using CIteratorT = typename PrimeType::const_iterator;
        std::function<void(CIteratorT, int64_t, NumType const &, string)>
            traverseThePrime = [&](CIteratorT iterPrime, int64_t count, NumType const & prevPart, string view)
        {
            OneDigitNumType currPrime = *iterPrime;
            if (iterPrime == primes.begin()) { // TODO: replace to if constexpr()
                NumType whole = prevPart * pow<OneDigitNumType, int64_t, NumType>(currPrime, count);
                string outStr = //view + string(count, '0' + *iterPrime) 
                   // + "\n" 
                    //+ 
                    whole.to_string(BASE);
                bool flag = true;
                for (int j = 0; j < outStr.length(); j++)
                    if (outStr[j] == '0') {
                        flag = false;
                        break;
                    }
                if (flag)
                    cout << counter++ << ") " << outStr << endl;
            }
            else {
                NumType tempPrime = 1;
                for (int64_t i = 0; i <= count; i++) {
                    NumType nextPart = prevPart * tempPrime;
                    traverseThePrime(std::prev(iterPrime), count - i, nextPart, view);
                    tempPrime *= OneDigitNumType(currPrime);
                    view += std::to_string(*iterPrime);
                }
            }
        };

        // std::find_if for not only InputIterator
        auto iterFirst = primes.begin();
        for (; iterFirst != primes.end(); iterFirst++) {
            if (*iterFirst >= BASE)
                break;
        }
        iterFirst--;
        
        for (int64_t len = 2; len <= MAX_LEN; len++) {
            traverseThePrime(iterFirst, len, NumType(1), "");
        }

        cout << endl;










        //vector<int64_t> maxStepsVec;
        //vector<MaxNumType> maxNumberVec;

        //vector<int64_t> stepsStat(13, 0);
        const OneDigitNumType SEVEN = 7;
        const OneDigitNumType THREE = 3;
        const OneDigitNumType FIVE = 5;
        const OneDigitNumType TWO = 2;
        const OneDigitNumType DEC = 10;
        const OneDigitNumType ZERO = 0;

        auto updateMaxSteps = [&ZERO, &DEC](int64_t twos,
            int64_t threes, int64_t fives, int64_t sevens,
            int64_t& maxSteps, MaxNumType& maxNumber, TArray& nums
            //, auto& maxStepsVec, auto& maxNumberVec
            )
        {
            auto convertToNumber = [&]()
            {
                maxNumber = MaxNumType(0);
                for (int64_t i = 0; i < (twos % 3) % 2; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(2);
                }
                for (int64_t i = 0; i < threes % 2; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(3);
                }
                for (int64_t i = 0; i < (twos % 3) / 2; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(4);
                }
                for (int64_t i = 0; i < fives; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(5);
                }
                for (int64_t i = 0; i < sevens; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(7);
                }
                for (int64_t i = 0; i < twos / 3; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(8);
                }
                for (int64_t i = 0; i < threes / 2; i++) {
                    maxNumber *= OneDigitNumType(10);
                    maxNumber += OneDigitNumType(9);
                }
            };
            // TODO: need optimization!
            int64_t iNum = 0;
            for (; ; iNum++) {
                nums[iNum + 1] = NumType(1);

                auto curr = nums[iNum];
                // TODO: optimize: make divide method with self-altering (/=, but with returning remainder)
                while (curr > ZERO) {
                    auto res = curr.divide(NumType(10));
                    nums[iNum + 1] *= res.second;
                    curr = res.first;
                }

                if (nums[iNum + 1] < DEC)
                    break;
            }

            //++stepsStat[iNum + 2];

            /*if (iNum + 2 >= 9) {
                convertToNumber();
                maxNumberVec.push_back(maxNumber);
                maxStepsVec.push_back(iNum + 2);
            }*/
            if (maxSteps <= iNum + 2) {
                maxSteps = iNum + 2;
                convertToNumber();

                /*if (maxSteps == 11) {
                    cout << maxNumber.to_string() << endl;
                }*/
            }
        };


        //{
        //    for (int64_t len = 2; len <= MAX_LEN; len++)
        //    {
        //        NumType temp7 = 1;
        //        for (int64_t iSeven = 0; iSeven <= len; iSeven++)
        //        {
        //            NumType temp3 = 1;
        //            for (int64_t iThree = 0; iThree <= len - iSeven; iThree++)
        //            {
        //                NumType temp37 = temp7 * temp3;

        //                // twos = len - iSeven - iThree

        //                // multiply the digits

        //                auto iTwo = len - iSeven - iThree;

        //                numsPrivate[0] = temp37 *
        //                    pow<OneDigitNumType, int64_t, NumType>(TWO, iTwo);

        //                updateMaxSteps(iTwo, iThree, 0, iSeven, maxStepsPrivate,
        //                    maxNumberPrivate, numsPrivate
        //                    //, maxStepsVecPrivate, maxNumberVecPrivate
        //                );

        //                // fives = len - iSeven - iThree

        //                auto iFive = len - iSeven - iThree;
        //                numsPrivate[0] = temp37 *
        //                    pow<OneDigitNumType, int64_t, NumType>(FIVE, iFive);

        //                updateMaxSteps(0, iThree, iFive, iSeven, maxStepsPrivate,
        //                    maxNumberPrivate, numsPrivate
        //                );

        //                temp3 *= THREE;

        //            }

        //            temp7 *= SEVEN;

        //        }

        //    }

        //}

        cout << "Max steps: " << maxSteps << endl
            << "Number: " << maxNumber.to_string() << endl;

        /*cout << "list: " << endl;
        for (int i = 0; i < int(maxStepsVec.size()); i++) {
            auto maxNum = maxNumberVec[i];
            cout << maxNum.to_string() << "\t\t - " << maxStepsVec[i] << endl;

            int j = 1;
            for (; ; j++) {
                MaxNumType res(1);
                for (int i = 0; ; i++) {
                    if (maxNum <= MaxNumType(0))
                        break;
                    auto pair = maxNum.divide(MaxNumType(10));
                    auto digit = pair.second;
                    maxNum = pair.first;
                    res = res * MaxNumType(digit);
                }
                maxNum = res;
                cout << res.to_string() << endl;
                if (res <= MaxNumType(10))
                    break;
            }
        }

        cout << "Max decimal-digit count of type: "
            << int(CONTAINER_DIMENSION * 32. * std::log(2) / std::log(10)) << endl;
        cout << "Max steps: " << maxSteps << endl
            << "Number: " << maxNumber.to_string() << endl;

        for (int j = 0; ; j++) {
            MaxNumType res(1);
            for (int i = 0; ; i++) {
                if (maxNumber <= MaxNumType(0))
                    break;
                auto pair = maxNumber.divide(MaxNumType(10));
                auto digit = pair.second;
                maxNumber = pair.first;
                res = res * MaxNumType(digit);
            }
            maxNumber = res;
            cout << res.to_string() << endl;
            if (res <= MaxNumType(10))
                break;
        }

        cout << "1st step of last number: " << endl;
        std::copy(nums.begin(), std::next(nums.begin()), std::ostream_iterator<NumType>(cout, "\n"));

        cout << "Statistics - count of steps:" << endl;
        for (int i = 2; i < int(stepsStat.size()); i++)
            cout << "(" << i << ") - " << stepsStat[i] << endl;*/

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    }

}
