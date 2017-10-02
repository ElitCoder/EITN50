#include <sys/stat.h>
#include <sys/types.h>
#include <tss/platform.h>
#include <tss/tss_defines.h>
#include <tss/tss_typedef.h>
#include <tss/tss_structs.h>
#include <tss/tspi.h>
#include <trousers/trousers.h>
#include <tss/tss_error.h>

#include <string.h>

int main()
{
	TSS_HCONTEXT	h_context;
	TSS_HTPM		h_tpm;
	TSS_RESULT		result;
	TSS_HKEY		h_srk;
	TSS_HPOLICY		h_srk_policy;
	TSS_UUID		srk_uuid = TSS_UUID_SRK;	
	BYTE			wks[20];

	memset(wks, 0, 20);

	result = Tspi_Context_Create(&h_context);
	printf("created context with result: %s\n", Trspi_Error_String(result));

	result = Tspi_Context_Connect(h_context, NULL);
	printf("connected to tpm with result: %s\n", Trspi_Error_String(result));

	result = Tspi_Context_GetTpmObject(h_context, &h_tpm);
	printf("get tpm handle with result: %s\n", Trspi_Error_String(result));

	result = Tspi_Context_LoadKeyByUUID(h_context, TSS_PS_TYPE_SYSTEM, srk_uuid, &h_srk);

	

	Tspi_Context_FreeMemory(h_context, NULL);
	
	return 0;
}