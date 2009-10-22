package com.lightningtoads.examples.Performance;

import android.test.ActivityInstrumentationTestCase;

/**
 * This is a simple framework for a test of an Application.  See
 * {@link android.test.ApplicationTestCase ApplicationTestCase} for more information on
 * how to write and extend Application tests.
 * <p/>
 * To run this test, you can type:
 * adb shell am instrument -w \
 * -e class com.lightningtoads.examples.Performance.PerformanceTest \
 * com.lightningtoads.examples.Performance.tests/android.test.InstrumentationTestRunner
 */
public class PerformanceTest extends ActivityInstrumentationTestCase<Performance> {

    public PerformanceTest() {
        super("com.lightningtoads.examples.Performance", Performance.class);
    }

}
