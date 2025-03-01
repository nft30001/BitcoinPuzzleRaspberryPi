#include <chrono>
#include <iostream>
#include "Key.h"
#include "Point.h"
#include "sha256.h"
#include "ripemd160.h"
#include "test.h"

#define TEST(name, result, iterations, call) if (result) { std::cout << "* Passed test: " << name; Timer timer; if (iterations) { for (int i = 0; i < iterations; i++) call; std::cout << " [" << timer.stop(iterations) << " ns]"; } std::cout << std::endl; } else { std::cout << "* Failed test: " << name << std::endl; return -1; }

unsigned long long Timer::time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Timer::Timer()
{
	timer = time();
}

unsigned long long Timer::stop(int iterations)
{
	unsigned long long newTime = time();
	unsigned long long returnTime = (newTime - timer) * (1000000000 / iterations) / 1000;
	timer = newTime;
	return returnTime;
}

double Timer::stop()
{
    unsigned long long newTime = time();
    double returnTime = (newTime - timer) / 1000.0;
    timer = newTime;
    return returnTime;
}

bool check(unsigned char* array1, unsigned char* array2, int length)
{
	for (int c = 0; c < length; c++)
		if (array1[c] != array2[c])
			return false;
	return true;
}

bool check(unsigned* array1, unsigned* array2, int length)
{
    for (int c = 0; c < length; c++)
        if (array1[c] != array2[c])
            return false;
    return true;
}

int test()
{
    std::cout << "TESTING:" << std::endl;
    {
        unsigned inputs[4][64] =
        {
            { 0x56F5256A, 0xEE7B1105, 0x09C359AB, 0xFE5671A5, 0x12618F3D, 0xDB1B8451, 0xDB003795, 0x4525A1B0, 0x58800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000108 },
            { 0xD61C0C73, 0x45E9D99D, 0x0835F946, 0x18F6F060, 0x6314E91C, 0xB8CD396D, 0x7E456DD7, 0xCB1954B2, 0x13800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000108 },
            { 0x6B882009, 0x877C5BBF, 0x92FCD6FC, 0x0AE7741F, 0xC0B8148C, 0x5484F5D4, 0xEFDF78A0, 0x50C619ED, 0x13800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000108 },
            { 0x7FA8302F, 0x0146D868, 0x3EEA6ABF, 0xCA32DA5C, 0x1E9A151B, 0xD3AAF8F5, 0xE1EA9433, 0x1606122F, 0x13800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000108 }
        };
		unsigned outputs[4][8] =
        {
            { 0xD61C0C73, 0x45E9D99D, 0x0835F946, 0x18F6F060, 0x6314E91C, 0xB8CD396D, 0x7E456DD7, 0xCB1954B2 },
            { 0x6B882009, 0x877C5BBF, 0x92FCD6FC, 0x0AE7741F, 0xC0B8148C, 0x5484F5D4, 0xEFDF78A0, 0x50C619ED },
            { 0x7FA8302F, 0x0146D868, 0x3EEA6ABF, 0xCA32DA5C, 0x1E9A151B, 0xD3AAF8F5, 0xE1EA9433, 0x1606122F },
            { 0xE0C660E5, 0x8C0134D1, 0x362B7730, 0x9DA5F38E, 0x9320E07E, 0x186BA9F9, 0x90DE1179, 0x35D6A81A }
        };
		unsigned testOutputs[4][8];
		bool result = true;
		for (int i = 0; i < 4; i++)
        {
            sha256(inputs[i], testOutputs[i]);
		    result &= check(testOutputs[i], outputs[i], 8);
        }
		TEST("sha256", result, 1000000, sha256(inputs[0], testOutputs[0]))
		//sha256Time /= 4;
	}
	{
        unsigned input[8] = { 0x68656C6C, 0x6F2C206D, 0x79206E61, 0x6D652069, 0x73205669, 0x6B746F72, 0x20597573, 0x6B6F7679 };
		unsigned output[5] = { 0x7ED5EEF7, 0x78281CEB, 0x44536309, 0xABDA5680, 0xDF342508 };
		unsigned testInput[8];
		unsigned testOutput[5];
		memcpy(testInput, input, sizeof(testInput));
		for (int i = 0; i < 5; i++)
		{
			ripemd160(testInput, testOutput);
			//memcpy(testInput, testOutput, sizeof(testOutput));
			for (int j = 0; j < 5; j++)
            {
			    REVERSE(*((unsigned*)testOutput + j), &testInput[j])
            }
			testInput[5] = 0x00010203;
			testInput[6] = 0x04050607;
			testInput[7] = 0x08090A0B;
		}
		TEST("ripemd160", check(testOutput, output, 5), 1000000, ripemd160(testInput, testOutput))
	}
	{
		unsigned long long blocks[4] = {0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC};
		Key key(blocks[0], blocks[1], blocks[2], blocks[3]);
		bool result = true;
		for (int b = 0; b < 4; b++)
			result &= key.blocks[b] == blocks[b];
		TEST("Key(4 * unsigned long long)", result, 0, Key(blocks[0], blocks[1], blocks[2], blocks[3]))
	}
	{
		unsigned long long blocks[8] = {0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC, 0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465 };
		Key key(blocks[0], blocks[1], blocks[2], blocks[3], blocks[4], blocks[5], blocks[6], blocks[7]);
		bool result = true;
		for (int b = 0; b < 8; b++)
			result &= key.blocks[b] == blocks[b];
		TEST("Key(8 * unsigned long long)", result, 0, Key(blocks[0], blocks[1], blocks[2], blocks[3], blocks[4], blocks[5], blocks[6], blocks[7]))
	}
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        TEST("Key::compare", key1.compare(key2) > 0 && key2.compare(key1) < 0 && key2.compare(key3) == 0, 1000000, key2.compare(key3))
    }
    {
        Key key1(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465, 0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC, 0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC, 0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        TEST("Key::compareExtended", key1.compareExtended(key2) > 0 && key2.compareExtended(key1) < 0 && key2.compareExtended(key3) == 0, 0, key2.compareExtended(key3))
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC, 0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465, 0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key3(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465, 0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        TEST("Key::operator==", !(key1 == key2) && !(key2 == key1) && key2 == key3, 1000000, key2 == key3)
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0x678C3872C9464398, 0x7D3B91EA7EF401DD, 0x716C6AF7F674A9E4, 0xC9EC63F2DD01ED2F);
        int iterations = 10000000;
        for (int i = 0; i < iterations; i++)
            key1.add(key2);
        TEST("Key::add", key1 == key3, iterations, key1.add(key2))
    }
    {
        Key key1(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465, 0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC, 0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0x7B6980DED72FB0B8, 0xD361C11E9A968377, 0xB08CAE14FE36344F, 0x92E0DB6AD94B6BF6, 0x678C3872C98ED437, 0x7D3B91EA7EF401DD, 0x716C6AF7F674A9E4, 0xC9EC63F2DD01ED2F);
        int iterations = 10000000;
        for (int i = 0; i < iterations; i++)
            key1.addExtended(key2);
        TEST("Key::addExtended", key1.compareExtended(key3) == 0, iterations, key1.addExtended(key2))
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0x4C58CA4364A9EB98, 0x87FC67CBDCA84FD5, 0x39D45A33A6996C29, 0x2990690B16B78A29);
        int iterations = 10000000;
        for (int i = 0; i < iterations; i++)
            key1.subtract(key2);
        TEST("Key::subtract", key1 == key3, iterations, key1.subtract(key2))
    }
	{
		Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
		Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
		Key key3(0x67B745DB6D946D80, 0x7D3B91EA7EF401DD, 0x716C6AF7F674A9E4, 0xC9EC63F2DD01ED2F);
		Key key4(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
		Key key5(0xA60D7EA3E907F497, 0xFD640324D231D726, 0xAA5F9D6A3178F4F8, 0x8641998106234453);
		Key key6(0x0000000000001000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
		key4 += key5;
		int iterations = 10000000;
		for (int i = 0; i < iterations; i++)
			key1 += key2;
		TEST("Key::operator+=", key1 == key3 && key4 == key6, 0, key1 += key2)
	}
	{
		Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
		Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
		Key key3(0x4C2DBCDAC05BC1B0, 0x87FC67CBDCA84FD5, 0x39D45A33A6996C29, 0x2990690B16B78A29);
		int iterations = 10000000;
		for (int i = 0; i < iterations; i++)
			key1 -= key2;
		TEST("Key::operator-=", key1 == key3, iterations, key1 -= key2)
	}
	{
		Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
		Key key2(0x9C47D08FFB10D4B9, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
		Key key3(0xCB7E46648E52ECD8, 0x7F8A35359E58E71D, 0xC4717BD6F876BC58, 0xA31070A04A66455C, 0x61167D213C1F060D, 0x55B1DC22E97AF112, 0x319CA3EED777A46F, 0x225989DBBC349B6F);
		Key key4(0xDBEBF0133A2D2798, 0x352388C741DA4DE2, 0x47028045AEE1BC71, 0xA3A91A2749B03DA2, 0x2A640C92C21C9C1C, 0xDAD1BD6CA1C22F43, 0xAC8BDA2A53B2E97A, 0x17B0ED2BC3D07BB8);
		key1.multiply(key2, key1);
		bool result1 = key1.compareExtended(key3) == 0;
		int iterations = 999999;
		for (int i = 0; i < iterations; i++)
			key1.multiply(key2, key1);
		bool result2 = key1.compareExtended(key4) == 0;
		TEST("Key::multiply", result1 && result2, iterations, key1.multiply(key2, key1))
	}
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key3(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        int iterations = 1000000;
        for (int i = 0; i < iterations; i++)
            key1.multiply(key3, key1);
        for (int i = 0; i < iterations; i++)
            key2.multiplyReduced(key3, key2);
        TEST("Key::multiplyReduced()", key1.compare(key2) == 0, iterations, key2.multiplyReduced(key3, key2))
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        int iterations = 1000000;
        for (int i = 0; i < iterations; i++)
            key1.multiply(Key::R2, key1);
        for (int i = 0; i < iterations; i++)
            key2.multiplyByR2(key2);
        TEST("Key::multiplyByR2()", key1.compareExtended(key2) == 0, iterations, key2.multiplyByR2(key2))
    }
    {
        Key key1(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465, 0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0xC526CE3E29B569A1, 0xF55B7994186165D1, 0xD227C08AA6818A39, 0x03F4520816A42A72);
        int iterations = 1000000;
        for (int i = 0; i < iterations; i++)
        {
            key1.reduce();
            key1.blocks[4] = key1.blocks[0];
            key1.blocks[5] = key1.blocks[1];
            key1.blocks[6] = key1.blocks[2];
            key1.blocks[7] = key1.blocks[3];
        }
        TEST("Key::reduce", key1 == key2, iterations, key1.reduce())
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Key key3(0xEF029439A9EDBC13, 0x7E8E80CFE1DFE00B, 0x99C0D7B247E7EC50, 0xC8DB3296835A12B0);
        int iterations = 100000;
        for (int i = 0; i < iterations; i++)
            key1 *= key2;
        TEST("Key::operator*=", key1 == key3, iterations, key1 *= key2)
    }
    {
        Key dividend1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key divisor1(0x000007A2000E90A1, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000);
        Key divisorCopy1 = divisor1;
        Key quotient1(0x5FF029BB5143E173, 0x9519AC6ECE9B718E, 0x79BE62DDBAA72E10, 0x0000000000000000);
        Key quotient_1;
        Key remainder1(0x5C11A35A782C9E45, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
        dividend1.divide(divisor1, quotient_1);
        bool result1 = divisor1 == divisorCopy1 && dividend1 == remainder1 && quotient_1 == quotient1;
        Key dividend2(0xFFFFFFFEFFFFFC2F, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
        Key divisor2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key divisorCopy2 = divisor2;
        Key quotient2(0x0000000000000002, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
        Key quotient_2;
        Key remainder2(0x4C1AFD48D20FCCFF, 0xFAC80649A463AE4D, 0x54BF3AD462F1E9F1, 0x0C8333020C4688A7);
        dividend2.divide(divisor2, quotient_2);
        bool result2 = divisor2 == divisorCopy2 && dividend2 == remainder2 && quotient_2 == quotient2;
        TEST("Key::divide", result1 && result2, 10000, dividend1.divide(divisor1, quotient_1))
    }
    {
        Key a1(0x0000000000000003, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
        Key b1(0x000000000000000B, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
        Key x1(0x0000000000000004, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000);
        Key y1(0xFFFFFFFEFFFFFC2E, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
        Key x_1;
        Key y_1;
        Key::gcd(a1, b1, x_1, y_1);
        bool result1 = x_1 == x1 && y_1 == y1;
        Key a2(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key b2(0xFFFFFFFEFFFFFC2F, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
        Key x2(0xE61D003687CA9EF6, 0x67B8E794ABFB076B, 0x870AAEB8AD77626A, 0x237AFDF1D2938D86);
        Key y2(0xB946138B35F7D60E, 0x7935C9F605369F4E, 0xC5ADA7DDB91DC968, 0xEF207A7B9FA568F6);
        Key x_2;
        Key y_2;
        Key::gcd(a2, b2, x_2, y_2);
        bool result2 = x_2 == x2 && y_2 == y2;
        TEST("Key::gcd", result1 && result2, 100, Key::gcd(a2, b2, x_2, y_2))
    }
    {
        Key key1(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key key2(0xE61D003687CA9EF6, 0x67B8E794ABFB076B, 0x870AAEB8AD77626A, 0x237AFDF1D2938D86);
        Key key = key1;
        bool result = true;
        key.invert();
        result &= key == key2;
        key.invert();
        result &= key == key1;
        TEST("Key::invert", result, 100, key.invert())
    }
    {
        Key inputs1[Key::GROUP_SIZE / 2 + 1];
        Key inputs2[Key::GROUP_SIZE / 2 + 1];
        inputs1[0] = Key(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        inputs2[0] = inputs1[0];
        for (int k = 1; k < Key::GROUP_SIZE / 2 + 1; k++)
        {
            inputs1[k] = inputs1[k - 1];
            inputs1[k] += Key::ONE;
            inputs2[k] = inputs1[k];
        }
        Key::invertGroup(inputs2);
        bool result = true;
        for (int k = 0; k < Key::GROUP_SIZE / 2 + 1; k++)
        {
            inputs1[k].invert();
            result &= inputs1[k] == inputs2[k];
        }
        TEST("Key::invertGroup", result, 10, Key::invertGroup(inputs2))
    }
    {
        //unsigned long long key = 1;
        Key x(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC);
        Key y(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465);
        Point point(x, y);
        TEST("Point(unsigned long long, Key, Key)", point.x == x && point.y == y, 0, Point(x, y))
    }
    {
        Point point1 = Point(/*1, */Key(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC), Key(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465));
        Point point2 = point1;
        Point point3 = Point(/*1025, */Key(0x8304214FE4985A86, 0x640D2464C7FE0AC4, 0x066535A04DBF563A, 0x635CD7A05064D3BC), Key(0xE0453E27E9E3AE1F, 0xE8D5F047F3281A4C, 0x46735CFCAF9E2F30, 0x00E40265913E77F6));
        TEST("Point::operator==", point1 == point2 && !(point1 == point3), 0, point1 == point2)
    }
    {
        Point point1 = Point(/*1, */Key(0x59F2815B16F81798, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC), Key(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465));
        Point point2 = Point(/*0, */Key(0x9C47D08FFB10D4B8, 0xFD17B448A6855419, 0x5DA4FBFC0E1108A8, 0x483ADA7726A3C465), Key(0x59F2815B16F81799, 0x029BFCDB2DCE28D9, 0x55A06295CE870B07, 0x79BE667EF9DCBBAC));
        unsigned compression1[9] = { 0x0279BE66, 0x7EF9DCBB, 0xAC55A062, 0x95CE870B, 0x07029BFC, 0xDB2DCE28, 0xD959F281, 0x5B16F817, 0x98800000 };
        unsigned compression2[9] = { 0x03483ADA, 0x7726A3C4, 0x655DA4FB, 0xFC0E1108, 0xA8FD17B4, 0x48A68554, 0x199C47D0, 0x8FFB10D4, 0xB8800000 };
        unsigned testCompression1[9];
        unsigned testCompression2[9];
        point1.compress(testCompression1);
        point2.compress(testCompression2);
        TEST("Point::compress", check(testCompression1, compression1, 9) && check(testCompression2, compression2, 9), 1000000, point1.compress(testCompression1))
    }
    {
        Point point1 = Point(/*1024, */Key(0xD5B901B2E285131F, 0xAAEC6ECDC813B088, 0xD664A18F66AD6240, 0x241FEBB8E23CBD77), Key(0xABB3E66F2750026D, 0xCD50FD0FBD0CB5AF, 0xD6C420BD13981DF8, 0x513378D9FF94F8D3));
        Point point2 = Point(/*1025, */Key(0x8304214FE4985A86, 0x640D2464C7FE0AC4, 0x066535A04DBF563A, 0x635CD7A05064D3BC), Key(0xE0453E27E9E3AE1F, 0xE8D5F047F3281A4C, 0x46735CFCAF9E2F30, 0x00E40265913E77F6));
        Key inverse = Point::G.x;
        inverse -= point1.x;
        inverse.invert();
        Point temp;
        point1.add(Point::G, inverse, temp);
        TEST("Point::add", temp == point2, 10000, point1.add(Point::G, inverse, temp))
    }
    {
        Point point1 = Point(/*1025, */Key(0x8304214FE4985A86, 0x640D2464C7FE0AC4, 0x066535A04DBF563A, 0x635CD7A05064D3BC), Key(0xE0453E27E9E3AE1F, 0xE8D5F047F3281A4C, 0x46735CFCAF9E2F30, 0x00E40265913E77F6));
        Point point2 = Point(/*1024, */Key(0xD5B901B2E285131F, 0xAAEC6ECDC813B088, 0xD664A18F66AD6240, 0x241FEBB8E23CBD77), Key(0xABB3E66F2750026D, 0xCD50FD0FBD0CB5AF, 0xD6C420BD13981DF8, 0x513378D9FF94F8D3));
        Key inverse = Point::G.x;
        inverse -= point1.x;
        inverse.invert();
        Point temp;
        point1.subtract(Point::G, inverse, temp);
        TEST("Point::subtract", temp == point2, 10000, point1.subtract(Point::G, inverse, temp))
    }
    {
        Point point1 = Point(/*1024, */Key(0xD5B901B2E285131F, 0xAAEC6ECDC813B088, 0xD664A18F66AD6240, 0x241FEBB8E23CBD77), Key(0xABB3E66F2750026D, 0xCD50FD0FBD0CB5AF, 0xD6C420BD13981DF8, 0x513378D9FF94F8D3));
        Point point2 = Point(/*1025, */Key(0x8304214FE4985A86, 0x640D2464C7FE0AC4, 0x066535A04DBF563A, 0x635CD7A05064D3BC), Key(0xE0453E27E9E3AE1F, 0xE8D5F047F3281A4C, 0x46735CFCAF9E2F30, 0x00E40265913E77F6));
        point1 += Point::G;
        TEST("Point::operator+=", point1 == point2, 0, point1 += Point::G)
    }
    {
        Point point1 = Point::G;
        Point point2 = Point(/*1024, */Key(0xD5B901B2E285131F, 0xAAEC6ECDC813B088, 0xD664A18F66AD6240, 0x241FEBB8E23CBD77), Key(0xABB3E66F2750026D, 0xCD50FD0FBD0CB5AF, 0xD6C420BD13981DF8, 0x513378D9FF94F8D3));
        for (int i = 0; i < 10; i++)
            point1.double_();
        TEST("Point::double_", point1 == point2, 0, point1.double_())
    }
    {
        Point gPower46 = Point(/*70368744177664, */Key(0x87EDA8BAB4E218DA, 0x0F4C85F152686050, 0xE68F17D8FF41C259, 0x13D1FFC481509BEE), Key(0xE0DB419DDB191C19, 0xA4AD01206D5BD127, 0xCECB9337B1B758BD, 0x6008391FA991961D));
        Point gMultiple81 = Point(/*82, */Key(0x9E594FECC13B59DF, 0xBE89B397B454C8B5, 0x0A37C28E771C6CB4, 0xE35BC6BB1B05B213), Key(0xC128B757CDD92ACB, 0x358EB4E66A331B76, 0x9C4D07D56A198DEC, 0x21868874CC2CB5A7));
        Point::initialize();
        TEST("Point::initialize", Point::gPowers[46] == gPower46 && Point::gMultiples[81] == gMultiple81, 0, Point::initialize())
    }
    {
        unsigned long long key = 0xA6CB2E5A34934C75;
        Point point1(key);
        Point point2;
        //point2.key = key;
        point2.x = Key(0x16B16A4B7CDD836E, 0x9C54414F3FFD46BD, 0x22E04C5F74753CF8, 0xB74CDBB3469D900F);
        point2.y = Key(0x3DB3590975F08733, 0x54299F35E9C79C77, 0x2107FFF206C80EF1, 0x6A952BA15E247C7C);
        TEST("Point(Key)", point1 == point2, 0, point1 = Point(key))
    }
    return 0;
}
