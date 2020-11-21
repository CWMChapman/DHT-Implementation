#include "functions.hpp"


int main_server_decode(std::array<uint8_t, 4>* message) {
    int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
    int bit_index = 0;   // which bit within each element of the array we are refering to
    int ret = 0;         // what we will return

    for (int i = 0; i < 32; ++i) {
        ret |= (((*message)[array_index] >> bit_index) & 1) << (31 - i);  

        if ((i + 1) % 8 == 0) {
             ++array_index;
            bit_index = 0;
        }
        else {
            ++bit_index;
        }
    }
    return ret;
}

void main_server_encode(std::array<uint8_t, 4>* message, int port) {
    int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
    int bit_index = 0;   // which bit within each element of the array we are refering to

    for (int i = 0; i < 32; ++i) {
        (*message)[array_index] &= ~(1 << bit_index);
        (*message)[array_index] |= ((port >> (31-i)) & 1) << bit_index;

        if ((i + 1) % 8 == 0) {
            ++array_index;
            bit_index = 0;
        }
        else {
            ++bit_index;
        }
    }
    return;
}



int decode_9byte(std::array<uint8_t, 9> *message, int section) {
	int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
	int bit_index = 0;	 // which bit within each element of the array we are refering to
	
	if (section == 0) {
		int action = 0;		 // message's action	-- section 0
		for (int i = 0; i < 8; ++i) {
			action |= (((*message)[array_index] >> (bit_index)) & 1) << (7-i);
			++bit_index;
		}
		return action;
	}
	else if (section == 1) {
		int key = 0;		 // message's key	-- section 1
		array_index = 1;
		for (int i = 8; i < 40; ++i) {
			key |= (((*message)[array_index] >> bit_index) & 1) << (31 + 8 - i);

			if ((i + 1) % 8 == 0) {
				++array_index;
				bit_index = 0;
			}
			else {
				++bit_index;
			}
		}
		return key;
	}
	else if (section == 2) {
		int value = 0;		 // message's value	-- section 2
		array_index = 5;
		for (int i = 40; i < 72; ++i) {
			value |= (((*message)[array_index] >> bit_index) & 1) << (31 + 40 - i);

			if ((i + 1) % 8 == 0) {
				++array_index;
				bit_index = 0;
			}
			else {
				++bit_index;
			}
		}
		return value;
	}
	else {
		std::cout << "ERROR: That section does not exist, please choose section 0 through 2." << std::endl;
		return -1;
	}
}

void encode_9byte(std::array<uint8_t, 9> *message, int action, int key, int value) {
	int array_index = 0; // remember that the array is of lenght 4, not 4 * 8.
	int bit_index = 0;	 // which bit within each element of the array we are refering to
	
	for (int i = 0; i < 8; ++i) {
		(*message)[array_index] |= ((action >> i) & 1) << (7-bit_index);
		++bit_index;
	}
	++array_index;
	for (int i = 8; i < 40; ++i) {
		(*message)[array_index] &= ~(1 << bit_index);
		(*message)[array_index] |= ((key >> (31 + 8 - i)) & 1) << bit_index;

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}
	for (int i = 40; i < 72; ++i) {
		(*message)[array_index] &= ~(1 << bit_index);
		(*message)[array_index] |= ((value >> (31 + 40 - i)) & 1) << bit_index;

		if ((i + 1) % 8 == 0) {
			++array_index;
			bit_index = 0;
		}
		else {
			++bit_index;
		}
	}
	return;
}