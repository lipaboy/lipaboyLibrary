#include "long_digits_multiplication_searching.h"

namespace lipaboy_lib::numberphile {

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


    // linux: <20, 1e60> - 2 secs, <20, 1e160> - 1,5 mins, <30, 1e240> - 22 mins
    // windows: <20, 1e90> - 2,25 mins (updated), <30, 1e40> - 13 secs (updated)
    void long_digits_multiplication_searching_long_numbers()
    {
        using IntType = LongIntegerDecimal<30>;
        // info uint64_t = 64 bit, 10^19 max value, as 7 is max value, then maximum 7^22
        constexpr uint64_t MAX = 40;
        using OneDigitIntType = 
            //IntType;
            LongIntegerDecimal<1>;

        auto startTime = extra::getCurrentTime();

        vector<IntType> nums(30, IntType(1));
        uint64_t maxSteps = 0;
        IntType maxNumber(1);

        auto updateMaxSteps = [&maxSteps, &maxNumber, &nums] (uint64_t twos,
                uint64_t threes, uint64_t fives, uint64_t sevens)
        {
            // TODO: need optimization!
            uint64_t iNum = 0;
            for (; ; iNum++) {
                nums[iNum + 1] = IntType(1);

                auto curr = nums[iNum];
                for (int i = 0; curr > IntType(0); i++) {
                    nums[iNum + 1] *= IntType(curr.divideByDec());
                }

                if (nums[iNum + 1] < IntType(10))
                    break;
            }

            if (maxSteps <= iNum + 2) {
                maxSteps = iNum + 2;
                maxNumber = IntType(0);
                for (uint64_t i = 0; i < twos % 2; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(2);
                }
                for (uint64_t i = 0; i < (twos % 3) / 2; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(4);
                }
                for (uint64_t i = 0; i < twos / 3; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(8);
                }
                for (uint64_t i = 0; i < threes % 2; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(3);
                }
                for (uint64_t i = 0; i < fives; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(5);
                }
                for (uint64_t i = 0; i < sevens; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(7);
                }
                for (uint64_t i = 0; i < threes / 2; i++) {
                    maxNumber *= IntType(10);
                    maxNumber += IntType(9);
                }
            }
        };

        using special::pow;

        const OneDigitIntType SEVEN = 7;
        const OneDigitIntType THREE = 3;
        const OneDigitIntType FIVE = 5;
        const OneDigitIntType TWO = 2;
        for (uint64_t len = 2; len <= MAX; len++) {

            IntType temp7 = 1;
            for (uint64_t iSeven = 0; iSeven <= len; iSeven++) {
                IntType temp3 = 1;
                //#pragma omp parallel for num_threads(2) private(nums)
                for (uint64_t iThree = 0; iThree <= len - iSeven; iThree++) {
                    IntType temp37 = temp7 * temp3;
                    //IntType temp37 = temp7 * pow<OneDigitIntType, uint64_t, IntType>(THREE, iThree);

                    // twos = len - iSeven - iThree

                    // multiply the digits

                    auto iTwo = len - iSeven - iThree;
                    nums[0] = temp37 *
                            pow<OneDigitIntType, uint64_t, IntType>(TWO, iTwo);

                    updateMaxSteps(iTwo, iThree, 0, iSeven);

                    // fives = len - iSeven - iThree

                    auto iFive = len - iSeven - iThree;
                    nums[0] = temp37 *
                            pow<OneDigitIntType, uint64_t, IntType>(FIVE, iFive);

                    updateMaxSteps(0, iThree, iFive, iSeven);

                    temp3 *= THREE;

                }

                temp7 *= SEVEN;

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

        cout << "Time elapsed: " << extra::diffFromNow(startTime) << endl;

    }


}

