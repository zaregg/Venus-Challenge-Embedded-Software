#include <json-c/json.h>
#include <stdio.h>

int main() {
    /* Create the main JSON object */
    json_object *jobj = json_object_new_object();
	int s = 8;
    /* Create and populate the sensor1 object */
    json_object *sensor1 = json_object_new_object();
    json_object_object_add(sensor1, "posx", json_object_new_int(10));
    json_object_object_add(sensor1, "posy", json_object_new_int(s));

    /* Create and populate the sensor2 object */
    json_object *sensor2 = json_object_new_object();
    json_object_object_add(sensor2, "posx", json_object_new_string("value3"));
    json_object_object_add(sensor2, "posy", json_object_new_string("value4"));

    /* Create and populate the robot object */
    json_object *robot = json_object_new_object();
    json_object_object_add(robot, "posx", json_object_new_string("value5"));
    json_object_object_add(robot, "posy", json_object_new_string("value6"));

    /* Create and populate the block object */
    json_object *block = json_object_new_object();
    json_object_object_add(block, "isitthere", json_object_new_boolean(1));
    json_object_object_add(block, "color", json_object_new_string("red"));

    /* Add the sensor1, sensor2, robot, and block objects to the main object */
    json_object_object_add(jobj, "D1", sensor1);
    json_object_object_add(jobj, "sensor2", sensor2);
    json_object_object_add(jobj, "robot", robot);
    json_object_object_add(jobj, "block", block);

    /* Print the JSON object */
    printf("%s\n", json_object_to_json_string(jobj));

    /* Free the main JSON object (frees all nested objects as well) */
    json_object_put(jobj);

    return 0;
}
