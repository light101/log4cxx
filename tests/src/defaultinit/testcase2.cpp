/*
 * Copyright 2003-2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG4CXX_TEST 1
#include <log4cxx/private/log4cxx_private.h>

#ifdef LOG4CXX_HAVE_XML

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <log4cxx/logmanager.h>
#include <log4cxx/logger.h>
#include "../insertwide.h"

using namespace log4cxx;

class TestCase2 : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE(TestCase2);
      CPPUNIT_TEST(xmlTest);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp()
   {
   }

   void tearDown()
   {
      LogManager::shutdown();
   }

   void xmlTest()
   {
      LoggerPtr root = Logger::getRootLogger();
      bool rootIsConfigured = !root->getAllAppenders().empty();
      CPPUNIT_ASSERT(rootIsConfigured);

      AppenderList list = root->getAllAppenders();
      AppenderPtr appender = list.front();
      CPPUNIT_ASSERT_EQUAL((LogString) LOG4CXX_STR("D1"), appender->getName());
   }

};

CPPUNIT_NS::Test* createTestCase2() {
   return TestCase2::suite();
}


#endif //LOG4CXX_HAVE_XML
