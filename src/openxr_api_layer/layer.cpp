#include "layer.h"
#include <string>
#include "utils/log.h"
using namespace openxr_api_layer::log;


namespace openxr_api_layer {

	PFN_xrGetInstanceProcAddr nextGetInstanceProcAddr = NULL;

	XrInstance xrInstance;
	
	XRAPI_ATTR XrResult XRAPI_CALL my_xrApplyHapticFeedback(
		XrSession session,
		const XrHapticActionInfo* hapticActionInfo,
		const XrHapticBaseHeader* hapticFeedback) {

		PFN_xrApplyHapticFeedback next;

		XrResult result = nextGetInstanceProcAddr(
			xrInstance,
			"xrApplyHapticFeedback",
			(PFN_xrVoidFunction*)&next
		);

		if (XR_FAILED(result)) {
			ErrorLog("Failed to get xrApplyHapticFeedback from next layer\n");
			return result;
		}

		if (hapticFeedback->type == XR_TYPE_HAPTIC_VIBRATION) {
			const XrHapticVibration* vibration = (const XrHapticVibration*)hapticFeedback;
						
			DebugLog("XrHapticVibration hooked! Freq: %f Hz, Ampl: %f, Duration: %d\n", vibration->frequency, vibration->amplitude, vibration->duration);
			
			if (vibration->frequency == 0.f) {
				// zero frequency is transmitted to the driver as 200Hz, which causes noise on the Valve Index controllers.
				DebugLog("Overriding zero frequency!\n");
				XrHapticVibration modifiedVibration = *vibration;
				modifiedVibration.frequency = 20.f;

				return next(session, hapticActionInfo, (XrHapticBaseHeader*)&modifiedVibration);
			}

		}

		return next(session, hapticActionInfo, hapticFeedback);
	}

	XrResult XRAPI_PTR my_xrDestroyInstance(XrInstance instance) {
		Log("Destroying layer %s\n", LAYER_NAME);
		stopLogging();

		PFN_xrDestroyInstance next;

		XrResult result = nextGetInstanceProcAddr(
			xrInstance,
			"xrDestroyInstance",
			(PFN_xrVoidFunction*)&next
		);

		if (XR_FAILED(result)) {
			ErrorLog("Failed to get xrDestroyInstance from next layer\n");
			return result;
		}

		return next(instance);
	}


	XRAPI_ATTR XrResult my_xrGetInstanceProcAddr(XrInstance instance, const char* name, PFN_xrVoidFunction* function)
	{
		// hooking some methods.
		if (strcmp(name, "xrApplyHapticFeedback") == 0) {
			*function = (PFN_xrVoidFunction)my_xrApplyHapticFeedback;
			return XR_SUCCESS;
		}
		else if (strcmp(name, "xrDestroyInstance") == 0) {
			*function = (PFN_xrVoidFunction)my_xrDestroyInstance;
			return XR_SUCCESS;
		}
		else {
			return nextGetInstanceProcAddr(instance, name, function);
		}
	}

	XrResult XRAPI_PTR my_xrCreateApiLayerInstance(
		const XrInstanceCreateInfo* info,
		const XrApiLayerCreateInfo* apiLayerInfo,
		XrInstance* instance)
	{
		Log("Creating api layer instance.\n");

		if (!apiLayerInfo || apiLayerInfo->structType != XR_LOADER_INTERFACE_STRUCT_API_LAYER_CREATE_INFO ||
			apiLayerInfo->structVersion != XR_API_LAYER_CREATE_INFO_STRUCT_VERSION ||
			apiLayerInfo->structSize != sizeof(XrApiLayerCreateInfo) || !apiLayerInfo->nextInfo ||
			apiLayerInfo->nextInfo->structType != XR_LOADER_INTERFACE_STRUCT_API_LAYER_NEXT_INFO ||
			apiLayerInfo->nextInfo->structVersion != XR_API_LAYER_NEXT_INFO_STRUCT_VERSION ||
			apiLayerInfo->nextInfo->structSize != sizeof(XrApiLayerNextInfo) || !apiLayerInfo->nextInfo->layerName ||
			!apiLayerInfo->nextInfo->nextGetInstanceProcAddr || !apiLayerInfo->nextInfo->nextCreateApiLayerInstance) {
			ErrorLog("xrCreateApiLayerInstance validation failed\n");
			return XR_ERROR_INITIALIZATION_FAILED;
		}


		XrResult result;

		result = apiLayerInfo->nextInfo->nextCreateApiLayerInstance(info, apiLayerInfo, instance);

		if (XR_FAILED(result)) {
			ErrorLog("Failed to load create api layer instance: %d\n", result);
			return result;
		}

		xrInstance = *instance;

		nextGetInstanceProcAddr = apiLayerInfo->nextInfo->nextGetInstanceProcAddr;		

		Log("Api layer instance created.\n");
		return XR_SUCCESS;
	}
}