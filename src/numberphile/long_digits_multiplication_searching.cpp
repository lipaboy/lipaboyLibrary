#include <omp.h>
#include <iterator>
#include <cmath>
#include <fstream>

#include "long_digits_multiplication_searching.h"
#include "stream/stream.h"

namespace lipaboy_lib::numberphile {

    // Results:
    //
    // 437799 - 7 steps (22 3 3333 77)
    // 248883999 - 9 steps
    // 3778888999 - 10 steps
    // 27777789999999999 - 11 steps
    // 288888877777799  - 11 steps of digit-multiplications
    //
    // https://zen.yandex.ru/media/tehno_chtivo/v-chem-chislo-277777788888899-mirovoi-rekordsmen-5ede6acd9abc2748d3bbf7e2
    //



    // linux: <1e60> - 0,3 secs, <1e160> - 12 secs, <1e300> - 33 mins
    // <1e150, 8 ths> - 6,3 secs, <1e200, 8 ths> - 12,8 secs, <1e400, 8ths> - 6,8 mins
    // <1e500, 6 ths> - 18,6 mins, <1e600, 6 ths> - 51,6 mins, <1e700, 6ths> - 58,3 mins
    // <1e700-705, 6ths> - 6,5 mins
    // windows: <1e90> - 2,25 mins, <1e40> - 13 secs

    void long_digits_multiplication_searching_long_numbers()
    {
        // PLAN:
        // 1) make type for MaxNumber
        // 2) add continue for calculating

        #define NUM_THREADS 6
        constexpr int64_t FIRST_LEN = 2;
        constexpr int64_t MAX_LEN = 20;
        constexpr size_t CONTAINER_DIMENSION = MAX_LEN / 9 + ((MAX_LEN % 9 == 0) ? 0 : 1);

        using IntType = LongIntegerDecimal<CONTAINER_DIMENSION>;
        using OneDigitIntType =
            LongIntegerDecimal<1>;
        using TArray = std::array<IntType, 16>;
        using special::pow;

        auto startTime = extra::getCurrentTime();

        TArray nums;
        nums.fill(IntType(1));
        int64_t maxSteps = 0;
        IntType maxNumber(1);
        vector<int64_t> maxStepsVec;
        vector<IntType> maxNumberVec;

        vector<int64_t> stepsStat(13, 0);

        const OneDigitIntType SEVEN = 7;
        const OneDigitIntType THREE = 3;
        const OneDigitIntType FIVE = 5;
        const OneDigitIntType TWO = 2;
        const OneDigitIntType DEC = 10;

        auto updateMaxSteps = [&DEC, &stepsStat](int64_t twos,
            int64_t threes, int64_t fives, int64_t sevens,
            int64_t& maxSteps, IntType& maxNumber, TArray& nums,
            auto& maxStepsVec, auto& maxNumberVec)
        {
            auto convertToNumber = [&] (IntType& maxNumber)
            {
                maxNumber = IntType(0);
                for (int64_t i = 0; i < (twos % 3) % 2; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(2);
                }
                for (int64_t i = 0; i < threes % 2; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(3);
                }
                for (int64_t i = 0; i < (twos % 3) / 2; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(4);
                }
                for (int64_t i = 0; i < fives; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(5);
                }
                for (int64_t i = 0; i < sevens; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(7);
                }
                for (int64_t i = 0; i < twos / 3; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(8);
                }
                for (int64_t i = 0; i < threes / 2; i++) {
                    maxNumber *= OneDigitIntType(10);
                    maxNumber += IntType(9);
                }
            };

            if (twos == 2 && threes == 3 && sevens == 14)
                bool kek = true;

            // TODO: need optimization!
            int64_t iNum = 0;
            for (; ; iNum++) {
                nums[iNum + 1] = IntType(1);

                auto curr = nums[iNum];
                OneDigitIntType remainder;
                int i;
                for (i = int(curr.length()) - 1; curr[i] <= 0; i--) {}

                for(auto part = curr[i]; part > 0; part /= 10) {
                    remainder = part % 10;
                    nums[iNum + 1] *= remainder;
                }
                for (i--; i >= 0; i--) {
                    auto part = curr[i];
                    for(uint32_t j = 0; j < IntType::integralModulusDegree(); j++) {
                        remainder = part % 10;
                        nums[iNum + 1] *= remainder;
                        part /= 10;
                    }
                }

                if (nums[iNum + 1] < DEC)
                    break;
            }

            ++stepsStat[iNum + 2];


            if (maxSteps <= iNum + 2) {
                maxSteps = iNum + 2;
                convertToNumber(maxNumber);

                if (maxSteps == 11) {
                    if (maxNumber.to_string() == "34777777777777779")
                        bool kek = true;
                    cout << maxNumber << endl;
                }
            }
//            if (iNum + 2 == 8) {
//                IntType number;
//                convertToNumber(number);
//                maxNumberVec.push_back(number);
//                maxStepsVec.push_back(iNum + 2);
//            }
        };

#pragma omp parallel num_threads(NUM_THREADS)
        {
            //cout << omp_get_num_threads() << endl;
            int64_t maxStepsPrivate = 0;
            IntType maxNumberPrivate(1);
            vector<int64_t> maxStepsVecPrivate;
            vector<IntType> maxNumberVecPrivate;
            TArray numsPrivate;
            numsPrivate.fill(IntType(1));

            // <35, 1e60> 1 thread - 13 secs, 4 threads - 9 secs, 8 threads - 7,5 secs, 8 threads + static,1 = 4 secs
            // <35, 1e90> 8 threads, dynamic,1 - 17 secs, static,1 - 18,7 secs, static,1,collapse2 - 17,7 secs
#pragma omp for nowait schedule(dynamic, 1) //collapse(2)
            for (int64_t len = FIRST_LEN; len <= MAX_LEN; len++)
            {
                IntType temp7 = 1;
                for (int64_t iSeven = 0; iSeven <= len; iSeven++)
                {
                    // INFO: uncomment if you using collapse in omp parallelism
                    //IntType temp7 = pow<OneDigitIntType, uint64_t, IntType>(SEVEN, iSeven);
                    IntType temp3 = 1;
                    for (int64_t iThree = 0; iThree <= len - iSeven; iThree++)
                    {
                        if (iThree == 3 && iSeven == 14 && len == 19)
                            bool kek = true;
                        IntType temp37 = temp7 * temp3;
                        //IntType temp37 = temp7 * pow<OneDigitIntType, uint64_t, IntType>(THREE, iThree);

                        // twos = len - iSeven - iThree

                        // multiply the digits

                        auto iTwo = len - iSeven - iThree;


                        numsPrivate[0] = temp37 *
                            pow<OneDigitIntType, int64_t, IntType>(TWO, iTwo);

                        updateMaxSteps(iTwo, iThree, 0, iSeven, maxStepsPrivate,
                                       maxNumberPrivate, numsPrivate, maxStepsVecPrivate, maxNumberVecPrivate);

                        // fives = len - iSeven - iThree

                        auto iFive = len - iSeven - iThree;
                        numsPrivate[0] = temp37 *
                            pow<OneDigitIntType, int64_t, IntType>(FIVE, iFive);

                        updateMaxSteps(0, iThree, iFive, iSeven, maxStepsPrivate,
                                       maxNumberPrivate, numsPrivate, maxStepsVecPrivate, maxNumberVecPrivate);

                        temp3 *= THREE;

                    }

                    temp7 *= SEVEN;

                }

            }


#pragma omp critical 
            {
                if (maxSteps < maxStepsPrivate) {
                    maxSteps = maxStepsPrivate;
                    maxNumber = maxNumberPrivate;
                    nums = numsPrivate;
                }
                maxStepsVec.insert(maxStepsVec.end(), maxStepsVecPrivate.begin(), maxStepsVecPrivate.end());
                maxNumberVec.insert(maxNumberVec.end(), maxNumberVecPrivate.begin(), maxNumberVecPrivate.end());
            }

        }
        cout << "list: " << endl;
        for (int i = 0; i < int(maxStepsVec.size()); i++) {
            auto maxNum = maxNumberVec[i];
            cout << maxNum << "\t\t - " << maxStepsVec[i] << endl;

            int j = 1;
            for (; ; j++) {
                IntType res(1);
                for (int i = 0; ; i++) {
                    if (maxNum <= IntType(0))
                        break;
                    auto digit = maxNum.divideByDec();
                    res = res * IntType(digit);
                }
                maxNum = res;
                cout << res.to_string() << endl;
                if (res <= IntType(10))
                    break;
            }
        }

        cout << "Max decimal-digit count of type: "
            << IntType::integralModulusDegree() * IntType::length() << endl;
        cout << "Max steps: " << maxSteps << endl
            << "Number: " << maxNumber.to_string() << endl;

        for (int j = 0; ; j++) {
            IntType res(1);
            for (int i = 0; ; i++) {
                if (maxNumber <= IntType(0))
                    break;
                auto digit = maxNumber.divideByDec();
                res = res * IntType(digit);
            }
            maxNumber = res;
            cout << res.to_string() << endl;
            if (res <= IntType(10))
                break;
        }

        cout << "1st step of last number: " << endl;
        std::copy(nums.begin(), std::next(nums.begin()), std::ostream_iterator<IntType>(cout, "\n"));

        cout << "Statistics - count of steps:" << endl;
        for (int i = 2; i < int(stepsStat.size()); i++)
            cout << "(" << i << ") - " << stepsStat[i] << endl;

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    }


    void long_digits_multiplication_searching_long_unsigned()
    {
        #define NUM_THREADS_UNSIGNED 1
        constexpr int64_t MAX_LEN = 20;
        // = MAX_LEN * log(7) / log(2^32) + 1
        constexpr int64_t CONTAINER_DIMENSION = int64_t(2.81 * double(MAX_LEN) / 32.) + 1;
        using NumType = LongUnsigned<CONTAINER_DIMENSION>;
        // = MAX_LEN / (log(2^32) / log(10)) + 1
        constexpr int64_t MAX_NUMBER_CONTAINER_DIMENSION = int64_t(double(MAX_LEN) / 0.3 / 32.) + 1;
        using MaxNumType = LongUnsigned<MAX_NUMBER_CONTAINER_DIMENSION>;

        // info uint64_t = 64 bit, 10^19 max value, as 7 is max value, then maximum 7^22
        using OneDigitNumType =
            //IntType;
            LongUnsigned<1>;
        using TArray = std::array<NumType, 30>;
        using special::pow;

        auto startTime = extra::getCurrentTime();

        TArray nums;
        nums.fill(NumType(1));
        int64_t maxSteps = 0;
        MaxNumType maxNumber(1);
        vector<int64_t> maxStepsVec;
        vector<MaxNumType> maxNumberVec;

        vector<int64_t> stepsStat(13, 0);

        const OneDigitNumType SEVEN = 7;
        const OneDigitNumType THREE = 3;
        const OneDigitNumType FIVE = 5;
        const OneDigitNumType TWO = 2;
        const OneDigitNumType DEC = 10;
        const OneDigitNumType ZERO = 0;

        auto updateMaxSteps = [&ZERO, &DEC, &stepsStat](int64_t twos,
            int64_t threes, int64_t fives, int64_t sevens,
            int64_t& maxSteps, MaxNumType& maxNumber, TArray& nums,
            auto& maxStepsVec, auto& maxNumberVec)
        {
            auto convertToNumber = [&] ()
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
//                IntType remainder;
                while(curr > ZERO) {
                    auto res = curr.divide(NumType(10));
                    nums[iNum + 1] *= res.second;
                    curr = res.first;
                }
//                for (int i = int(curr.length()) - 1; i >= 0; i--) {
//                    for(auto part = curr[i]; part > 0; part /= 10) {
//                        remainder = part % 10;
//                        nums[iNum + 1] *= remainder;
//                    }
//                }

                if (nums[iNum + 1] < DEC)
                    break;
            }

            ++stepsStat[iNum + 2];

            if (iNum + 2 >= 9) {
                convertToNumber();
                maxNumberVec.push_back(maxNumber);
                maxStepsVec.push_back(iNum + 2);
            }
            if (maxSteps <= iNum + 2) {
                maxSteps = iNum + 2;
                convertToNumber();

                if (maxSteps == 11) {
                    cout << maxNumber.to_string() << endl;
                }
            }
        };

        #pragma omp parallel num_threads(NUM_THREADS_UNSIGNED)
        {
            //cout << omp_get_num_threads() << endl;
            int64_t maxStepsPrivate = 0;
            MaxNumType maxNumberPrivate(1);
            vector<int64_t> maxStepsVecPrivate;
            vector<MaxNumType> maxNumberVecPrivate;
            TArray numsPrivate;
            numsPrivate.fill(NumType(1));

            // <35, 1e60> 1 thread - 13 secs, 4 threads - 9 secs, 8 threads - 7,5 secs, 8 threads + static,1 = 4 secs
            // <35, 1e90> 8 threads, dynamic,1 - 17 secs, static,1 - 18,7 secs, static,1,collapse2 - 17,7 secs
            #pragma omp for nowait schedule(dynamic, 1) //collapse(2)
            for (int64_t len = 2; len <= MAX_LEN; len++)
            {
                NumType temp7 = 1;
                for (int64_t iSeven = 0; iSeven <= len; iSeven++)
                {
                    // INFO: uncomment if you using collapse in omp parallelism
                    //IntType temp7 = pow<OneDigitIntType, uint64_t, IntType>(SEVEN, iSeven);
                    NumType temp3 = 1;
                    for (int64_t iThree = 0; iThree <= len - iSeven; iThree++)
                    {
                        NumType temp37 = temp7 * temp3;
                        //IntType temp37 = temp7 * pow<OneDigitIntType, uint64_t, IntType>(THREE, iThree);

                        // twos = len - iSeven - iThree

                        // multiply the digits

                        auto iTwo = len - iSeven - iThree;

                        numsPrivate[0] = temp37 *
                            pow<OneDigitNumType, int64_t, NumType>(TWO, iTwo);

                        updateMaxSteps(iTwo, iThree, 0, iSeven, maxStepsPrivate,
                                       maxNumberPrivate, numsPrivate, maxStepsVecPrivate, maxNumberVecPrivate);

                        // fives = len - iSeven - iThree

                        auto iFive = len - iSeven - iThree;
                        numsPrivate[0] = temp37 *
                            pow<OneDigitNumType, int64_t, NumType>(FIVE, iFive);

                        updateMaxSteps(0, iThree, iFive, iSeven, maxStepsPrivate,
                                       maxNumberPrivate, numsPrivate, maxStepsVecPrivate, maxNumberVecPrivate);

                        temp3 *= THREE;

                    }

                    temp7 *= SEVEN;

                }

            }


            #pragma omp critical
            {
                if (maxSteps < maxStepsPrivate) {
                    maxSteps = maxStepsPrivate;
                    maxNumber = maxNumberPrivate;
                    nums = numsPrivate;
                }
                maxStepsVec.insert(maxStepsVec.end(), maxStepsVecPrivate.begin(), maxStepsVecPrivate.end());
                maxNumberVec.insert(maxNumberVec.end(), maxNumberVecPrivate.begin(), maxNumberVecPrivate.end());
            }

        }

        cout << "list: " << endl;
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
            << int(CONTAINER_DIMENSION * 32. * std::log(2) / std::log(10))<< endl;
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
            cout << "(" << i << ") - " << stepsStat[i] << endl;

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    }


    void long_digits_multiplication_searching_factorization() {
        using stream_space::Stream;
        using namespace stream_space::operators;

        auto startTime = extra::getCurrentTime();

        // find all the primes numbers till sqrt(998888887777772)

        using ContainerOfPrimes = vector<uint64_t>;
        //ContainerOfPrimes primes;

        int64_t number = 
            // 277777788888899;
            //12777; // 5! / 3!
            // 122333333777777;
            // 1099511627776;
            // 1277777788888899;
            // 11277777788888899;
            // 2247777778888899;
            // 22227777778888899;
            // 222224777777888899;
//               222233777777888889;
                334468899;
//                233777778999999999;
        auto getDigit = [&number](int index) -> int
        {
            auto temp = number / powDozen<int64_t>(index);
            return temp % 10;
        }; 
        auto setDigit = [&number, &getDigit](int index, int digit) -> void
        {
            auto old = getDigit(index);
            number = number + int64_t(digit - old) * powDozen<int64_t>(index);
        };
        constexpr std::array<int, 4> primes{ 2, 3, 5, 7 };
        auto checkFact = [&number, &primes]() {
            auto temp = number;
            for (int k = 0; k < int(primes.size()); k++) {
                while (temp % primes[k] == 0) {
                    temp /= primes[k];
                }
            }
            if (temp < 10) {
                cout << number << endl;
            }
        };

        //cout << number << endl;
        int64_t counter = 1;
        int64_t limit = int64_t(1e8);
        int length = int(std::log(number * 1.) / std::log(10.)) + 1;

        checkFact();
        for (int i = 0; i < length - 1; ) {
            int right = getDigit(i);
            int left = getDigit(i + 1);
            if (left < right) {
                int next;
                int j = 0;
                for (; j < length; j++) {
                    next = getDigit(j);
                    if (next > left)
                        break;
                }
                //swap
                setDigit(i + 1, next);
                setDigit(j, left);
                for (int k = 0; k < (i + 1) / 2; k++) {
                    int first = getDigit(k);
                    int second = getDigit(i - k);
                    setDigit(k, second);
                    setDigit(i - k, first);
                }
                i = 0;
                //cout << number << endl;
                counter++;
                if (counter > limit) {
                    int64_t newLimit;
                    cout << "Time elapsed of factorization: " << extra::diffFromNow(startTime) << endl;
                    cout << "continue: ";
                    std::cin >> newLimit;
                    if (newLimit <= 0)
                        break;
                    limit += powDozen<int64_t>(newLimit);
                    startTime = extra::getCurrentTime();
                }

                checkFact();
            }
            else
                i++;
        }
        cout << "counter: " << counter << endl;
    }

    //==============================================================//
    //============        Other algorithms           ===============//
    //==============================================================//

    void long_digits_multiplication_searching_vectors() {

        auto startTime = extra::getCurrentTime();

            // initial values are important
        vector< vector<char> > temps(30, vector<char>(50, 1));
        temps[0] = vector<char>(50, 2);
        vector<size_t> sizes(30, 1);
        sizes[0] = 2;

        // (Linux) 1e8 - 12 secs, 1e9 - 116 secs,  1e15 > 5 min
        constexpr long long MAX = static_cast<long long>(1e2);
        size_t maxSteps = 0;
            vector<char> maxElem;
        for (size_t i = 0; i < MAX; i++) {

            // multiply the digits until we get the one digit as result

            size_t iTemp = 0;
            for (; ; iTemp++) {
                sizes[iTemp + 1] = 1;
                temps[iTemp + 1][0] = 1;

                for (size_t k = 0; k < sizes[iTemp]; k++) {
                                    if (temps[iTemp][k] == 1)
                                            continue;
                                    // k's digit
                    size_t t = 0;
                                    char remainder = 0; // decade's remainder
                                    // temps[iTemp + 1] - middle result of multiplication
                    for (; t < sizes[iTemp + 1]; t++) {
                                            char& curr = temps[iTemp + 1][t];
                                            curr = curr * temps[iTemp][k] + remainder;
                                            remainder = curr / 10;
                        curr %= 10;
                                    }
                    if (remainder > 0) {
                        sizes[iTemp + 1]++;
                        temps[iTemp + 1][t] = remainder;
                    }
                            }
                if (sizes[iTemp + 1] <= 1)
                    break;
                    }

                    vector<char>& number = temps[0];

            if (iTemp + 1 >= maxSteps) {
                            maxSteps = iTemp + 1;
                            maxElem = number;
                    }

                    // get next number

            size_t j = 0;
            for (; j < sizes[0]; j++) {
                            number[j]++;
                            if (number[j] < 10)
                                    break;
                number[j] = 2;
                    }
            if (j == sizes[0])
                sizes[0]++;

            }

            cout << "Max steps: " << maxSteps << endl
                    << "Number: ";
        auto vec = Stream(maxElem)
                    | map([](char ch) -> int { return int(ch); })
            | get(sizes[0])
            | to_vector();
        (Stream(vec.rbegin(), vec.rend()) | print_to(cout)) << endl;

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;


    }

    void long_digits_multiplication_searching_uint64_t() {
        auto startTime = extra::getCurrentTime();

        vector<int64_t> numbers(30, 1);
        numbers[0] = 11;

        // linux: 1e9 - 23 secs, 1e10 - 211 secs
        constexpr uint64_t MAX = static_cast<uint64_t>(1e2);
        uint64_t maxSteps = 0;
        uint64_t maxNumber = 1;
        for (uint64_t i = 0; i < MAX; i++) {

            // multiply the digits

            uint64_t iNum = 0;
            for (; ; iNum++) {
                numbers[iNum + 1] = 1;

                auto curr = numbers[iNum];
                for ( ; curr > 0; ) {
                    numbers[iNum + 1] *= curr % 10;
                    curr /= 10;
                }

                if (numbers[iNum + 1] < 10)
                    break;
            }

            if (maxSteps <= iNum + 1) {
                maxSteps = iNum + 1;
                maxNumber = numbers[0];
            }

            // get next number
            numbers[0]++;
        }

        cout << "Max steps: " << maxSteps << endl
            << "Number: " << maxNumber << endl;

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    //    string str;
    //    std::cin >> str;
    }

    void long_digits_multiplication_searching_optimized() {

        // Results: 277777788888899 get on MAX == 30

        auto startTime = extra::getCurrentTime();

        vector<uint64_t> nums(30, 1);

        // linux: 1e40 - 2 ms,
        // info uint64_t = 64 bit, 10^19 max value, as 7 is max value, then maximum 7^22
        constexpr uint64_t MAX = 5;
        uint64_t maxSteps = 0;
        uint64_t maxNumber = 1;

        auto updateMaxSteps = [&maxSteps, &maxNumber, &nums] (uint64_t twos,
                uint64_t threes, uint64_t fives, uint64_t sevens)
        {
            uint64_t iNum = 0;
            for (; ; iNum++) {
                nums[iNum + 1] = 1;

                auto curr = nums[iNum];
                for ( ; curr > 0; ) {
                    nums[iNum + 1] *= curr % 10;
                    curr /= 10;
                }

                if (nums[iNum + 1] < 10)
                    break;
            }

            if (maxSteps <= iNum + 2) {
                maxSteps = iNum + 2;
                maxNumber = 0;
                for (uint64_t i = 0; i < twos % 2; i++) {
                    maxNumber *= 10;
                    maxNumber += 2;
                }
                for (uint64_t i = 0; i < (twos % 3) / 2; i++) {
                    maxNumber *= 10;
                    maxNumber += 4;
                }
                for (uint64_t i = 0; i < twos / 3; i++) {
                    maxNumber *= 10;
                    maxNumber += 8;
                }
                for (uint64_t i = 0; i < threes % 2; i++) {
                    maxNumber *= 10;
                    maxNumber += 3;
                }
                for (uint64_t i = 0; i < fives; i++) {
                    maxNumber *= 10;
                    maxNumber += 5;
                }
                for (uint64_t i = 0; i < sevens; i++) {
                    maxNumber *= 10;
                    maxNumber += 7;
                }
                for (uint64_t i = 0; i < threes / 2; i++) {
                    maxNumber *= 10;
                    maxNumber += 9;
                }
            }
        };

        using special::pow;

        for (uint64_t len = 2; len <= MAX; len++) {

            for (uint64_t iTwo = 0; iTwo <= len; iTwo++) {

                for (uint64_t iThree = 0; iThree <= len - iTwo; iThree++) {

                    // sevens = len - iTwo - iThree

                    // multiply the digits

                    auto iSeven = len - iTwo - iThree;
                    nums[0] = pow<uint64_t>(2, iTwo) *
                            pow<uint64_t>(3, iThree) *
                            pow<uint64_t>(7, iSeven);

                    updateMaxSteps(iTwo, iThree, 0, iSeven);

                }

            }

            for (uint64_t iFive = 0; iFive <= len; iFive++) {

                for (uint64_t iThree = 0; iThree <= len - iFive; iThree++) {

                    // sevens = len - iFive - iThree

                    auto iSeven = len - iFive - iThree;

                    nums[0] = pow<uint64_t>(5, iFive) *
                            pow<uint64_t>(3, iThree) *
                            pow<uint64_t>(7, iSeven);

                    updateMaxSteps(0, iThree, iFive, iSeven);

                }

            }

        }

        cout << "Max steps: " << maxSteps << endl
            << "Number: " << maxNumber << endl;

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

//        string str;
//        std::cin >> str;

    }



}

