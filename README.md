Spark Core M2X API Client
=====================

The Spark Core library is used to send/receive data to/from [AT&amp;T's M2X service](https://m2x.att.com/) from the [Spark Core](https://www.spark.io/).

Getting Started
==========================
1. Signup for an [M2X Account](https://m2x.att.com/signup).
2. Obtain your _Master Key_ from the Master Keys tab of your [Account Settings](https://m2x.att.com/account) screen.
3. Create your first [Data Source Blueprint](https://m2x.att.com/blueprints) and copy its _Feed ID_.
4. Review the [M2X API Documentation](https://m2x.att.com/developer/documentation/overview).
5. Obtain an Spark Core and [set it up](http://docs.spark.io/start/).

Please consult the [M2X glossary](https://m2x.att.com/developer/documentation/glossary) if you have questions about any M2X specific terms.

How to Install the library
==========================

1. Dependency [jsonlite](https://github.com/citrusbyte/jsonlite-sparkcore)

  * In the [Spark Core IDE](https://www.spark.io/build), click the `Libraries` Icon
  * Search for `jsonlite` and click the found library
  * Click `Include in App` and choose your current working Application

2. M2X Library

  * In the [Spark Core IDE](https://www.spark.io/build), click the `Libraries` Icon
  * Search for `m2x` and click the found library
  * Click `Include in App` and choose your current working Application

3. Examples can be found by clicking on `m2x` under your Application's `Included Libraries` listing

Hardware Setup
==============

Sensor Setup
------------

Different sensors can be hooked up to a Spark Core board to provide different properties including temperatures, humidity, etc. You can use a breadboard as well as wires to connect different sensors to your Spark Core. For a detailed tutorial on connecting different sensors, please refer to the Spark Core [Hackster](http://spark.hackster.io/) page.


Variables used in Examples
==========================

In order to run the given examples, different variables need to be configured. We will walk through those variables in this section.

Network Configuration
---------------------

If you are using a Wifi Shield, the following variables need configuration:

```
char ssid[] = "<ssid>";
char pass[] = "<WPA password>";
```

Just fill in the SSID and password of the Wifi hotspot and you should be good to go.

For an Ethernet Shield, the following variables are needed:

```
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,17);
```

For a newer Ethernet Shield, the MAC address should be printed on a sticker on the shield. However, some sold Ethernet Shields have no MAC address shown on the board. In this case, you can use any MAC address, as long as it does not conflict with another network device within the same LAN.

The IP address here is only used when DHCP fails to give a valid IP address. It is recommended, though not required, to provide a unique IP address here.

M2X API Key
-----------

Once you [register](https://m2x.att.com/signup) for an AT&amp;T M2X account, an API key is automatically generated for you. This key is called a _Primary Master Key_ and can be found in the _Master Keys_ tab of your [Account Settings](https://m2x.att.com/account). This key cannot be edited nor deleted, but it can be regenerated. It will give you full access to all APIs.

However, you can also create a _Data Source API Key_ associated with a given Data Source (Feed). You can use the Data Source API key to access the streams belonging to that Data Source.

You can customize this variable in the following line in the examples:

```
char m2xKey[] = "<M2X access key>";
```

Feed ID
-------

A feed is associated with a data source, it is a set of data streams, such as streams of locations, temperatures, etc. The following line is needed to configure the feed used:

```
char feedId[] = "<feed id>";
```

Stream Name
------------

A stream in a feed is a set of time-series data points of a specific type (i.e. humidity, temperature). You can use the M2XStreamClient library to send stream values to the M2X API server, or receive stream values from the M2X API server. Use the following line to configure the stream if needed:

```
char streamName[] = "<stream name>";
```

Using the M2XStreamClient library
=========================

```
TCPClient client;
M2XStreamClient m2xClient(&client, m2xKey);
```

In the M2XStreamClient, four (4) types of API functions are provided:

* `send`: Send stream value to M2X
* `receive`: Receive stream value from M2X
* `updateLocation`: Send location value of a feed to M2X
* `readLocation`: Receive location values of a feed from M2X

Returned values
---------------

For all those functions, the HTTP status code will be returned if we can fulfill an HTTP request. For example, `200` will be returned upon success, `401` will be returned if we didn't provide a valid M2X API Key.

Otherwise, the following error codes will be used:

```
static const int E_NOCONNECTION = -1;
static const int E_DISCONNECTED = -2;
static const int E_NOTREACHABLE = -3;
static const int E_INVALID = -4;
static const int E_JSON_INVALID = -5;
```

Post stream value
-----------------

The following functions can be used to post a value to a stream, which belongs to a feed:

```
template <class T>
int post(const char* feedId, const char* streamName, T value);
```

Here we use C++ templates to generate functions for different types of values, feel free to use values of `float`, `int`, `long` or even `const char*` types here.

Post multiple values
--------------------

M2X also supports posting multiple values to multiple streams in one call, use the following function for this:

```
template <class T>
int postMultiple(const char* feedId, int streamNum,
                 const char* names[], const int counts[],
                 const char* ats[], T values[]);
```

Please refer to the comments in the source code on how to use this function. Essentially, you will need to provide the list of streams you want to post to, and values for each stream.

Fetch stream value
------------------

Since Spark Core boards contain very limited memory, we cannot put the whole returned string in memory, parse it into JSON representations and read what we want. Instead, we use a callback-based mechanism here. We parse the returned JSON string piece by piece. Whenever we got a new stream value point, we will call the following callback functions:

```
void (*stream_value_read_callback)(const char* at,
                                   const char* value,
                                   int index,
                                   void* context);

```

The implementation of the callback function is left for the user to fill in, you can read the value of the point in the `value` argument, and the timestamp of the point in the `at` argument. We even pass the index of this data point in the whole stream as well as a user-specified context variable to this function, so that you can perform different tasks.

To read the stream values, all you need to do is call this function:

```
int fetchValues(const char* feedId, const char* streamName,
                stream_value_read_callback callback, void* context,
                const char* startTime = NULL, const char* endTime = NULL,
                const char* limit = NULL);
```

Besides the feed ID and stream name, only the callback function and a user context needs to be specified. Optional filtering parameters such as start time, end time, and limits per call can also be used here.

Update Datasource Location
--------------------------

You can use the following function to update the location for a data source(feed):

```
template <class T>
int updateLocation(const char* feedId, const char* name,
                   T latitude, T longitude, T elevation);
```

Different from stream values, locations are attached to feeds rather than streams.

The reasons we are providing templated functions is due to floating point value precision: on most Spark Core boards, `double` is the same as `float`, i.e., a 32-bit (4-byte) single precision floating number. That means only 7 digits in the number are reliable. When we are using `double` here to represent latitude/longitude, it means that only 5 digits after the floating point is accurate, which means we can represent as accurate to ~1.1132m distance using `double` here. If you want to represent cordinates that are more specific, you need to use strings here.

Read Datasource Location
------------------------

Similar to reading stream values, we also use callback functions here. The only difference is that different parameters are used in the function:

```
void (*location_read_callback)(const char* name,
                               double latitude,
                               double longitude,
                               double elevation,
                               const char* timestamp,
                               int index,
                               void* context);

```

For memory space consideration, we only provide double-precision when reading locations. An index of the location points is also provided here together with a user-specified context.

The API is also slightly different, in that the stream name is not needed here:

```
int readLocation(const char* feedId, location_read_callback callback,
                 void* context);

```

Examples
========

We provide a series of examples that will help you get an idea of how to use the `M2XStreamClient` library to perform all kinds of tasks.

Note that the examples contain fictionary variables, and that they need to be configured as per the instructions above before running on your Spark Core board. Each of the examples here also needs either a Wifi Shield or an Ethernet Shield hooked up to your device.

In the `ExamplePost` a temperature sensor, a breadboard and 5 wires are also needed to get temperature data, you need to wire the board like the following:

[![Wiring](https://github.com/attm2x/m2x-spark-core/raw/master/docs/SparkCoreWiring.jpg)](docs/SparkCoreWiring.jpg)

ExamplePost
-------

This example shows how to post temperatures to M2X server.

ExamplePostMultiple
---------------

This example shows how to post multiple values to multiple streams in one API call.

ExampleFetchValues
--------------

This example reads stream values from M2X server. And prints the stream data point got to Serial interface. You can find the actual values in the Spark Core `Serial Monitor`.

ExampleUpdateLocation
-----------------

This one sends location data to M2X server. Idealy a GPS device should be used here to read the cordinates, but for simplicity, we just use pre-set values here to show how to use the API.

ExampleReadLocation
---------------

This one reads location data of a feed from M2X server, and prints them to Serial interfact. You can check the output in the `Serial Monitor` of the Spark Core IDE.

LICENSE
=======

This library is released under the MIT license. See [`LICENSE`](LICENSE) for the terms.
