// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <windows.h>

#include "utils/log.h"
#include "layer.h"
#include <regex>


using namespace openxr_api_layer;
using namespace openxr_api_layer::log;

extern "C" {

    // Entry point for the loader.
    XrResult __declspec(dllexport) XRAPI_CALL
        xrNegotiateLoaderApiLayerInterface(const XrNegotiateLoaderInfo* const loaderInfo,
            const char* const apiLayerName,
            XrNegotiateApiLayerRequest* const apiLayerRequest) {
        
        startLogging();

        Log("--> xrNegotiateLoaderApiLayerInterface\n");

        Log("Initializing layer %s\n", apiLayerName);


        std::wstring ws_projectName(PROJECT_NAME, PROJECT_NAME + std::strlen(PROJECT_NAME));

        std::wstring ws_vendor(VENDOR, VENDOR + std::strlen(VENDOR));

        if (apiLayerName && std::string_view(apiLayerName) != LAYER_NAME) {
            ErrorLog("Invalid apiLayerName \"%s\"\n", apiLayerName);
            return XR_ERROR_INITIALIZATION_FAILED;
        }
        
        std::wstring ws_regSubKey = L"SOFTWARE\\" + ws_vendor + L"\\" + ws_projectName;
        std::wstring ws_regValue(L"enableDebug");

        Log("Reading registry value \"%ls\" at \"%ls\"\n", ws_regSubKey.c_str(), ws_regValue.c_str());
        
        DWORD result{};
        DWORD dataSize = sizeof(result);

        auto rc = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            ws_regSubKey.c_str(),
            ws_regValue.c_str(),
            RRF_RT_REG_DWORD,
            nullptr,
            &result,
            &dataSize
        );

        if (rc != ERROR_SUCCESS) {
            std::string s_err = "Failed to read registry. Windows system error code: " + std::to_string(rc);
            ErrorLog("%s\n", s_err.c_str());
            return XR_ERROR_INITIALIZATION_FAILED;
        }



        if (result == 1) {
            EnableDebugLog();
            DebugLog("DebugLog enabled\n");
        }
        

        if (!loaderInfo || !apiLayerRequest || loaderInfo->structType != XR_LOADER_INTERFACE_STRUCT_LOADER_INFO ||
            loaderInfo->structVersion != XR_LOADER_INFO_STRUCT_VERSION ||
            loaderInfo->structSize != sizeof(XrNegotiateLoaderInfo) ||
            apiLayerRequest->structType != XR_LOADER_INTERFACE_STRUCT_API_LAYER_REQUEST ||
            apiLayerRequest->structVersion != XR_API_LAYER_INFO_STRUCT_VERSION ||
            apiLayerRequest->structSize != sizeof(XrNegotiateApiLayerRequest) ||
            loaderInfo->minInterfaceVersion > XR_CURRENT_LOADER_API_LAYER_VERSION ||
            loaderInfo->maxInterfaceVersion < XR_CURRENT_LOADER_API_LAYER_VERSION ||
            loaderInfo->maxInterfaceVersion > XR_CURRENT_LOADER_API_LAYER_VERSION ||
            loaderInfo->maxApiVersion < XR_CURRENT_API_VERSION || loaderInfo->minApiVersion > XR_CURRENT_API_VERSION) {
            ErrorLog("xrNegotiateLoaderApiLayerInterface validation failed\n");
            return XR_ERROR_INITIALIZATION_FAILED;
        }

        // Setup our layer to intercept OpenXR calls.
        apiLayerRequest->layerInterfaceVersion = XR_CURRENT_LOADER_API_LAYER_VERSION;
        apiLayerRequest->layerApiVersion = XR_CURRENT_API_VERSION;
        apiLayerRequest->getInstanceProcAddr = my_xrGetInstanceProcAddr;
        apiLayerRequest->createApiLayerInstance = my_xrCreateApiLayerInstance;

        Log("<-- xrNegotiateLoaderApiLayerInterface\n");

        return XR_SUCCESS;
    }
}
