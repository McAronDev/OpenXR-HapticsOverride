#include <openxr/openxr_platform.h>
#include <openxr/openxr_loader_negotiation.h>

namespace openxr_api_layer {

    XRAPI_ATTR XrResult XRAPI_CALL my_xrGetInstanceProcAddr(
        XrInstance instance,
        const char* name,
        PFN_xrVoidFunction* function
    );

    XrResult XRAPI_PTR my_xrCreateApiLayerInstance(
        const XrInstanceCreateInfo* createInfo,
        const XrApiLayerCreateInfo* apiLayerCreateInfo,
        XrInstance* instance
    );
}