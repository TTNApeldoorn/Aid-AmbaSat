// Payload decoder, AmbaSat project Apeldoorn
// some gyro functions are moved from sensor to payload decoder
// in order to save program space in sensor
// Author Marcel Meek


function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  
   
// bytes to float little endian
  function bytesToFloat(bytes) {
    var bits = bytes[3]<<24 | bytes[2]<<16 | bytes[1]<<8 | bytes[0];
    var sign = (bits>>>31 === 0) ? 1.0 : -1.0;
    var exponent = bits>>>23 & 0xff;
    var mantisse = (exponent === 0) ? (bits & 0x7fffff)<<1 : (bits & 0x7fffff) | 0x800000;
    var floating_point = sign * mantisse * Math.pow(2, exponent - 127) / 0x800000;
    return floating_point;
  }

// bytes to int32 little endian
  function bytesToInt32(bytes) {
      var val = bytes[3]<<24 | bytes[2]<<16 | bytes[1]<<8 | bytes[0];
     return (val < 0x80000000) ? val : val - 0x10000000;
   }
 
// bytes to int16 little endian
   function bytesToInt16(bytes) {
     var val = bytes[1]<<8 | bytes[0];
     return (val < 0x8000) ? val : val - 0x10000;
   }
  
// bytes to hex 
  function bytesToHex( bytes) {
    var hex = bytes[3].toString(16);
    hex += bytes[2].toString(16);
    hex += bytes[1].toString(16);
    hex += bytes[0].toString(16);
    return hex.toUpperCase();
  }

// limit floats to three decimales
  function threeDecimals( val) {
    return Math.round( val * 1000) / 1000;
  }


// define constants (copied from SparkFun LSM9DS1 IMU library)
  var DECLINATION = -0.62; // Declination (degrees), northallerton, North Yorks 
  var SENSITIVITY_ACCELEROMETER_2 = 0.000061;
  //const SENSITIVITY_ACCELEROMETER_4 = 0.000122;
  //const SENSITIVITY_ACCELEROMETER_8 = 0.000244;
  //const SENSITIVITY_ACCELEROMETER_16 = 0.000732;
  var SENSITIVITY_GYROSCOPE_245 = 0.00875;
  //const SENSITIVITY_GYROSCOPE_500 = 0.0175;
  //const SENSITIVITY_GYROSCOPE_2000 =0.07;
  var SENSITIVITY_MAGNETOMETER_4 = 0.00014;
  //const SENSITIVITY_MAGNETOMETER_8   0.00029;
  //const SENSITIVITY_MAGNETOMETER_12  0.00043;
  //const SENSITIVITY_MAGNETOMETER_16  0.00058;


// Calculate gyroscope pitch, roll, and heading.
// copied from AmbaSat cpp example
  function calcAttitude( ax, ay, az, mx, my ) {
    var obj = {};
    var roll = Math.atan2( ay, az);
    var pitch = Math.atan2(-ax, Math.sqrt(ay * ay + az * az));
    var heading;
    if (my === 0)
        heading = (mx < 0) ? Math.PI : 0;
    else
        heading = Math.atan2(mx, my);

    heading -= DECLINATION * Math.PI / 180;

    if (heading > Math.PI) heading -= (2 * Math.PI);
    else if (heading < -Math.PI) heading += (2 * Math.PI);

    // Convert everything from radians to degrees:
    heading *= 180.0 / Math.PI;
    pitch *= 180.0 / Math.PI;
    roll  *= 180.0 / Math.PI;
    
    obj.heading = threeDecimals( heading);
    obj.pitch = threeDecimals( pitch);
    obj.roll = threeDecimals(roll);
    return obj;
  }

 
// decode Amabast 2021 payload
 
  var decoded = {};
  if( port == 1) {
    
    decoded.lux = threeDecimals( bytesToFloat( bytes.slice(0)));
    decoded.vcc = bytesToInt16( bytes.slice(22)) / 1000;

    var gr = {};   // get gyro raw values
    gr.ax =  bytesToInt16( bytes.slice(4));
    gr.ay =  bytesToInt16( bytes.slice(6));
    gr.az =  bytesToInt16( bytes.slice(8));
    gr.gx =  bytesToInt16( bytes.slice(10));
    gr.gy =  bytesToInt16( bytes.slice(12));
    gr.gz =  bytesToInt16( bytes.slice(14));
    gr.mx =  bytesToInt16( bytes.slice(16));
    gr.my =  bytesToInt16( bytes.slice(18));
    gr.mz =  bytesToInt16( bytes.slice(20));
    decoded.gyro_raw = gr;
    
    var gc = {};    // calculate raw values to si values
    gc.ax = threeDecimals( gr.ax * SENSITIVITY_ACCELEROMETER_2);
    gc.ay = threeDecimals( gr.ay * SENSITIVITY_ACCELEROMETER_2);
    gc.az = threeDecimals( gr.az * SENSITIVITY_ACCELEROMETER_2);
    gc.gx = threeDecimals( gr.gx * SENSITIVITY_GYROSCOPE_245);
    gc.gy = threeDecimals( gr.gy * SENSITIVITY_GYROSCOPE_245);
    gc.gz = threeDecimals( gr.gz * SENSITIVITY_GYROSCOPE_245);
    gc.mx = threeDecimals( gr.mx * SENSITIVITY_MAGNETOMETER_4);
    gc.my = threeDecimals( gr.my * SENSITIVITY_MAGNETOMETER_4);
    gc.mz = threeDecimals( gr.mz * SENSITIVITY_MAGNETOMETER_4);
    decoded.gyro_calc = gc;
    
    // caculate pitch, roll and heading
    decoded.gyro_attitude = calcAttitude( gr.ax, gr.ay, gr.az, -gr.mx, -gr.my );
    
  }
  return decoded;
}    
