#include <XPLMDataAccess.h>

#include "adf_helper.h"
#include "conf.h"
#include "log.h"


namespace adf_helper
{
    // ADF
    XPLMDataRef dref_Adf1_Left_Freq;
    XPLMDataRef dref_Adf1_Right_Freq;

    XPLMDataRef dref_Adf2_Left_Freq;
    XPLMDataRef dref_Adf2_Right_Freq;

    XPLMDataRef dref_Adf1_Left_Freq_1;
    XPLMDataRef dref_Adf1_Left_Freq_2;
    XPLMDataRef dref_Adf1_Left_Freq_3;

    XPLMDataRef dref_Adf1_Right_Freq_1;
    XPLMDataRef dref_Adf1_Right_Freq_2;
    XPLMDataRef dref_Adf1_Right_Freq_3;

    XPLMDataRef dref_Adf2_Left_Freq_1;
    XPLMDataRef dref_Adf2_Left_Freq_2;
    XPLMDataRef dref_Adf2_Left_Freq_3;

    XPLMDataRef dref_Adf2_Right_Freq_1;
    XPLMDataRef dref_Adf2_Right_Freq_2;
    XPLMDataRef dref_Adf2_Right_Freq_3;

    // RMI
    XPLMDataRef dref_RMI_Left_Pointer1_Selector;
    XPLMDataRef dref_RMI_Left_Pointer2_Selector;
    XPLMDataRef dref_RMI_Right_Pointer1_Selector;
    XPLMDataRef dref_RMI_Right_Pointer2_Selector;

    XPLMDataRef dref_RMI_Left_Pointer1_Deg;
    XPLMDataRef dref_RMI_Left_Pointer2_Deg;
    XPLMDataRef dref_RMI_Right_Pointer1_Deg;
    XPLMDataRef dref_RMI_Right_Pointer2_Deg;

    XPLMDataRef dref_Adf_deg[2];

    XPLMDataRef reg(
        const char* str,
        int(*get_pfn)(void* inRefcon),
        void(*set_pfn)(void* inRefcon, int inValue),
        void* param)
    {
        return XPLMRegisterDataAccessor(
            str,
            xplmType_Int,
            set_pfn != NULL,                // inIsWritable
            get_pfn,                        // inReadInt
            set_pfn,                        // inWriteInt
            NULL,                           // inReadFloat
            NULL,                           // inWriteFloat
            NULL,                           // inReadDouble
            NULL,                           // inWriteDouble
            NULL,                           // inReadIntArray
            NULL,                           // inWriteIntArray
            NULL,                           // inReadFloatArray
            NULL,                           // inWriteFloatArray
            NULL,                           // inReadData
            NULL,                           // inWriteData
            param,                          // inReadRefcon
            param                           // inWriteRefcon
        );
    }

    XPLMDataRef reg(
        const char* str,
        float(*get_pfn)(void* inRefcon),
        void(*set_pfn)(void* inRefcon, float inValue),
        void* param)
    {
        return XPLMRegisterDataAccessor(
            str,
            xplmType_Float,
            set_pfn != NULL,                // inIsWritable
            NULL,                           // inReadInt
            NULL,                           // inWriteInt
            get_pfn,                        // inReadFloat
            set_pfn,                        // inWriteFloat
            NULL,                           // inReadDouble
            NULL,                           // inWriteDouble
            NULL,                           // inReadIntArray
            NULL,                           // inWriteIntArray
            NULL,                           // inReadFloatArray
            NULL,                           // inWriteFloatArray
            NULL,                           // inReadData
            NULL,                           // inWriteData
            param,                          // inReadRefcon
            param                           // inWriteRefcon
        );
    }


    template<int Method>
    int get_adf_freq(void* inRefcon)
    {
        int val = XPLMGetDatai(*reinterpret_cast<XPLMDataRef*>(inRefcon));
        int rem = val;
        if (Method >= 1)
        {
            val /= 100;
            rem %= 100;
        }
        if (Method >= 2)
        {
            val = rem / 10;
            rem %= 10;
        }
        if (Method >= 3)
        {
            val = rem * 2;
        }
        return val;
    }

    template<int Method>
    void set_adf_freq(void* inRefcon, int inValue)
    {
        int value = XPLMGetDatai(*reinterpret_cast<XPLMDataRef*>(inRefcon));

        int n100, n10, n05, rem;
        n100 = value / 100;
        rem = value % 100;
        n10 = rem / 10;
        rem = rem % 10;
        n05 = rem * 2;

        if (Method == 1)
            n100 = inValue;
        if (Method == 2)
            n10 = inValue;
        if (Method == 3)
            n05 = inValue;

        value = n100 * 100 + n10 * 10 + n05 / 2;

        XPLMSetDatai(*reinterpret_cast<XPLMDataRef*>(inRefcon), value);
    }

    // RMI

    enum RMIs
    {
        RMI_LEFT,
        RMI_RIGHT
    };

    enum Pointers
    {
        POINTER_1,
        POINTER_2
    };


    int rmi_pointer_sel[2][2] = {
        { 0, 1 },
        { 0, 1 },
    };

    template<RMIs RMI, Pointers P>
    int get_rmi_sel(void* inRefcon)
    {
        return rmi_pointer_sel[RMI][P];
    }

    template<RMIs RMI, Pointers P>
    void set_rmi_sel(void* inRefcon, int inValue)
    {
        rmi_pointer_sel[RMI][P] = inValue == 0 ? 0 : 1;
    }

    template<RMIs RMI, Pointers P>
    float get_rmi_deg(void* inRefcon)
    {
        int adf = rmi_pointer_sel[RMI][P];

        return XPLMGetDataf(dref_Adf_deg[adf]);
    }

    void registerDataRef()
    {
        dref_Adf1_Left_Freq = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_left_frequency_hz");
        dref_Adf1_Right_Freq = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_right_frequency_hz");
        dref_Adf2_Left_Freq = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_left_frequency_hz");
        dref_Adf2_Right_Freq = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_right_frequency_hz");

        dref_Adf_deg[0] = XPLMFindDataRef("sim/cockpit2/radios/indicators/adf1_relative_bearing_deg");
        dref_Adf_deg[1] = XPLMFindDataRef("sim/cockpit2/radios/indicators/adf2_relative_bearing_deg");


        dref_Adf1_Left_Freq_1 = reg(DREF_ADF1_LEFT_FREQUENCY_HZ_1, get_adf_freq<1>, set_adf_freq<1>, &dref_Adf1_Left_Freq);
        dref_Adf1_Left_Freq_2 = reg(DREF_ADF1_LEFT_FREQUENCY_HZ_2, get_adf_freq<2>, set_adf_freq<2>, &dref_Adf1_Left_Freq);
        dref_Adf1_Left_Freq_3 = reg(DREF_ADF1_LEFT_FREQUENCY_HZ_3, get_adf_freq<3>, set_adf_freq<3>, &dref_Adf1_Left_Freq);

        dref_Adf1_Right_Freq_1 = reg(DREF_ADF1_RIGHT_FREQUENCY_HZ_1, get_adf_freq<1>, set_adf_freq<1>, &dref_Adf1_Right_Freq);
        dref_Adf1_Right_Freq_2 = reg(DREF_ADF1_RIGHT_FREQUENCY_HZ_2, get_adf_freq<2>, set_adf_freq<2>, &dref_Adf1_Right_Freq);
        dref_Adf1_Right_Freq_3 = reg(DREF_ADF1_RIGHT_FREQUENCY_HZ_3, get_adf_freq<3>, set_adf_freq<3>, &dref_Adf1_Right_Freq);

        dref_Adf2_Left_Freq_1 = reg(DREF_ADF2_LEFT_FREQUENCY_HZ_1, get_adf_freq<1>, set_adf_freq<1>, &dref_Adf2_Left_Freq);
        dref_Adf2_Left_Freq_2 = reg(DREF_ADF2_LEFT_FREQUENCY_HZ_2, get_adf_freq<2>, set_adf_freq<2>, &dref_Adf2_Left_Freq);
        dref_Adf2_Left_Freq_3 = reg(DREF_ADF2_LEFT_FREQUENCY_HZ_3, get_adf_freq<3>, set_adf_freq<3>, &dref_Adf2_Left_Freq);

        dref_Adf2_Right_Freq_1 = reg(DREF_ADF2_RIGHT_FREQUENCY_HZ_1, get_adf_freq<1>, set_adf_freq<1>, &dref_Adf2_Right_Freq);
        dref_Adf2_Right_Freq_2 = reg(DREF_ADF2_RIGHT_FREQUENCY_HZ_2, get_adf_freq<2>, set_adf_freq<2>, &dref_Adf2_Right_Freq);
        dref_Adf2_Right_Freq_3 = reg(DREF_ADF2_RIGHT_FREQUENCY_HZ_3, get_adf_freq<3>, set_adf_freq<3>, &dref_Adf2_Right_Freq);

        // RMI
        dref_RMI_Left_Pointer1_Selector = reg(DREF_RMI_LEFT_POINTER_1_SELECTOR, get_rmi_sel<RMI_LEFT, POINTER_1>, set_rmi_sel<RMI_LEFT, POINTER_1>, NULL);
        dref_RMI_Left_Pointer2_Selector = reg(DREF_RMI_LEFT_POINTER_2_SELECTOR, get_rmi_sel<RMI_LEFT, POINTER_2>, set_rmi_sel<RMI_LEFT, POINTER_2>, NULL);
        dref_RMI_Right_Pointer1_Selector = reg(DREF_RMI_RIGHT_POINTER_1_SELECTOR, get_rmi_sel<RMI_RIGHT, POINTER_1>, set_rmi_sel<RMI_RIGHT, POINTER_1>, NULL);
        dref_RMI_Right_Pointer2_Selector = reg(DREF_RMI_RIGHT_POINTER_2_SELECTOR, get_rmi_sel<RMI_RIGHT, POINTER_2>, set_rmi_sel<RMI_RIGHT, POINTER_2>, NULL);

        dref_RMI_Left_Pointer1_Deg = reg(DREF_RMI_LEFT_POINTER_1_DEG, get_rmi_deg<RMI_LEFT, POINTER_1>, nullptr, nullptr);
        dref_RMI_Left_Pointer2_Deg = reg(DREF_RMI_LEFT_POINTER_2_DEG, get_rmi_deg<RMI_LEFT, POINTER_2>, nullptr, nullptr);
        dref_RMI_Right_Pointer1_Deg = reg(DREF_RMI_RIGHT_POINTER_1_DEG, get_rmi_deg<RMI_RIGHT, POINTER_1>, nullptr, nullptr);
        dref_RMI_Right_Pointer2_Deg = reg(DREF_RMI_RIGHT_POINTER_2_DEG, get_rmi_deg<RMI_RIGHT, POINTER_2>, nullptr, nullptr);
    }

    void unregisterDataRef()
    {
        XPLMUnregisterDataAccessor(dref_Adf1_Left_Freq_1);
        XPLMUnregisterDataAccessor(dref_Adf1_Left_Freq_2);
        XPLMUnregisterDataAccessor(dref_Adf1_Left_Freq_3);

        XPLMUnregisterDataAccessor(dref_Adf1_Right_Freq_1);
        XPLMUnregisterDataAccessor(dref_Adf1_Right_Freq_2);
        XPLMUnregisterDataAccessor(dref_Adf1_Right_Freq_3);

        XPLMUnregisterDataAccessor(dref_Adf2_Left_Freq_1);
        XPLMUnregisterDataAccessor(dref_Adf2_Left_Freq_2);
        XPLMUnregisterDataAccessor(dref_Adf2_Left_Freq_3);

        XPLMUnregisterDataAccessor(dref_Adf2_Right_Freq_1);
        XPLMUnregisterDataAccessor(dref_Adf2_Right_Freq_2);
        XPLMUnregisterDataAccessor(dref_Adf2_Right_Freq_3);

        // RMI

        XPLMUnregisterDataAccessor(dref_RMI_Left_Pointer1_Selector);
        XPLMUnregisterDataAccessor(dref_RMI_Left_Pointer2_Selector);
        XPLMUnregisterDataAccessor(dref_RMI_Right_Pointer1_Selector);
        XPLMUnregisterDataAccessor(dref_RMI_Right_Pointer2_Selector);

        XPLMUnregisterDataAccessor(dref_RMI_Left_Pointer1_Deg);
        XPLMUnregisterDataAccessor(dref_RMI_Left_Pointer2_Deg);
        XPLMUnregisterDataAccessor(dref_RMI_Right_Pointer1_Deg);
        XPLMUnregisterDataAccessor(dref_RMI_Right_Pointer2_Deg);
    }
}
