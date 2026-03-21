#include <sysutils/su_base.h>
#include <stdio.h>
#include <string.h>

// Check if SU_CIPHER functions are available
#ifdef SU_CIPHER_AVAILABLE
#include <sysutils/su_cipher.h>
#endif

int main()
{
	SUDevID dev;
	int ret;

	printf("==============================================\n");
	printf("Start testing sysutils APIs\n");
	printf("==============================================\n");

	// Test SU_Base_GetDevID
	printf("\n1. Testing SU_Base_GetDevID...\n");
	memset(&dev, 0, sizeof(SUDevID));
	ret = SU_Base_GetDevID(&dev);
	if (ret != 0) {
		printf("   SU_Base_GetDevID failed: %d\n", ret);
	} else {
		printf("   SU_Base_GetDevID success\n");
		printf("   CHIP ID: %s\n", dev.chr);
	}

#ifdef SU_CIPHER_AVAILABLE
	// Test SU_CIPHER_Init
	printf("\n2. Testing SU_CIPHER_Init...\n");
	ret = SU_CIPHER_Init();
	if (ret != 0) {
		printf("   SU_CIPHER_Init failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_Init success\n");
	}

	// Test SU_CIPHER_DES_Init
	printf("\n3. Testing SU_CIPHER_DES_Init...\n");
	ret = SU_CIPHER_DES_Init();
	if (ret != 0) {
		printf("   SU_CIPHER_DES_Init failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_DES_Init success\n");
	}

	// Test SU_CIPHER_CreateHandle
	int handle;
	printf("\n4. Testing SU_CIPHER_CreateHandle...\n");
	handle = SU_CIPHER_CreateHandle();
	if (handle < 0) {
		printf("   SU_CIPHER_CreateHandle failed: %d\n", handle);
	} else {
		printf("   SU_CIPHER_CreateHandle success, handle: %d\n", handle);
	}

	// Test SU_CIPHER_ConfigHandle
	IN_UNF_CIPHER_CTRL ctrl;
	memset(&ctrl, 0, sizeof(IN_UNF_CIPHER_CTRL));
	ctrl.key[0] = 0x11223344;
	ctrl.key[1] = 0x55667788;
	ctrl.key[2] = 0x99aabbcc;
	ctrl.key[3] = 0xddeeff00;
	ctrl.IV[0] = 0x00ffeedd;
	ctrl.IV[1] = 0xccbbaa99;
	ctrl.IV[2] = 0x88776655;
	ctrl.IV[3] = 0x44332211;
	ctrl.enDataLen = 16; // 16 bytes = 4 * 4 bytes
	ctrl.enAlg = IN_UNF_CIPHER_ALG_AES;
	ctrl.enBitWidth = IN_UNF_CIPHER_BIT_WIDTH_128BIT;
	ctrl.enWorkMode = IN_UNF_CIPHER_WORK_MODE_ECB;
	ctrl.enKeyLen = IN_UNF_CIPHER_KEY_AES_128BIT;

	printf("\n5. Testing SU_CIPHER_ConfigHandle...\n");
	printf("   Configuration parameters:\n");
	printf("   - Key: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		ctrl.key[0], ctrl.key[1], ctrl.key[2], ctrl.key[3]);
	printf("   - IV: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		ctrl.IV[0], ctrl.IV[1], ctrl.IV[2], ctrl.IV[3]);
	printf("   - Data length: %u bytes\n", ctrl.enDataLen);
	printf("   - Algorithm: %s\n", ctrl.enAlg == IN_UNF_CIPHER_ALG_AES ? "AES" : "DES");
	printf("   - Bit width: 128bit\n");
	printf("   - Work mode: %s\n", 
		ctrl.enWorkMode == IN_UNF_CIPHER_WORK_MODE_ECB ? "ECB" : 
		ctrl.enWorkMode == IN_UNF_CIPHER_WORK_MODE_CBC ? "CBC" : "OTHER");
	printf("   - Key length: 128bit\n");

	ret = SU_CIPHER_ConfigHandle(handle, &ctrl);
	if (ret != 0) {
		printf("   SU_CIPHER_ConfigHandle failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_ConfigHandle success\n");
	}

	// Test SU_CIPHER_Encrypt
	unsigned int src_data[4] = {0x12345678, 0x87654321, 0x11223344, 0x44332211};
	unsigned int enc_data[4] = {0};
	unsigned int dec_data[4] = {0};

	printf("\n6. Testing SU_CIPHER_Encrypt...\n");
	printf("   Original data: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		src_data[0], src_data[1], src_data[2], src_data[3]);
	ret = SU_CIPHER_Encrypt(handle, src_data, enc_data, 16);
	if (ret != 0) {
		printf("   SU_CIPHER_Encrypt failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_Encrypt success\n");
		printf("   Encrypted data: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
			enc_data[0], enc_data[1], enc_data[2], enc_data[3]);
	}

	// Test SU_CIPHER_Decrypt
	printf("\n7. Testing SU_CIPHER_Decrypt...\n");
	printf("   Encrypted data: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		enc_data[0], enc_data[1], enc_data[2], enc_data[3]);
	ret = SU_CIPHER_Decrypt(handle, enc_data, dec_data, 16);
	if (ret != 0) {
		printf("   SU_CIPHER_Decrypt failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_Decrypt success\n");
		printf("   Decrypted data: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
			dec_data[0], dec_data[1], dec_data[2], dec_data[3]);
		
		// Verify decrypted data matches original
		if (memcmp(src_data, dec_data, sizeof(src_data)) == 0) {
			printf("   Verification: Decrypted data matches original\n");
		} else {
			printf("   Verification: Decrypted data does not match original\n");
		}
	}

	// Test SU_CIPHER_DES_Test
	printf("\n8. Testing SU_CIPHER_DES_Test...\n");
	ret = SU_CIPHER_DES_Test();
	if (ret != 0) {
		printf("   SU_CIPHER_DES_Test failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_DES_Test success\n");
	}

	// Test SU_CIPHER_DestroyHandle
	printf("\n9. Testing SU_CIPHER_DestroyHandle...\n");
	ret = SU_CIPHER_DestroyHandle(handle);
	if (ret != 0) {
		printf("   SU_CIPHER_DestroyHandle failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_DestroyHandle success\n");
	}

	// Test SU_CIPHER_DES_Exit
	printf("\n10. Testing SU_CIPHER_DES_Exit...\n");
	ret = SU_CIPHER_DES_Exit();
	if (ret != 0) {
		printf("   SU_CIPHER_DES_Exit failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_DES_Exit success\n");
	}

	// Test SU_CIPHER_Exit
	printf("\n11. Testing SU_CIPHER_Exit...\n");
	ret = SU_CIPHER_Exit();
	if (ret != 0) {
		printf("   SU_CIPHER_Exit failed: %d\n", ret);
	} else {
		printf("   SU_CIPHER_Exit success\n");
	}
#else
	printf("\n2. SU_CIPHER functions not available in this build\n");
#endif

	printf("\n==============================================\n");
	printf("Testing completed\n");
	printf("==============================================\n");

	return 0;
}
