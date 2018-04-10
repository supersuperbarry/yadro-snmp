/**
 * @brief SNMP Tables implementation
 *
 * This file is part of phosphor-snmp project.
 *
 * Copyright (c) 2018 YADRO (KNS Group LLC)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: Alexander Filippov <a.filippov@yadro.com>
 */

#include "yadro_sensorstables.hpp"
#include "sensors.hpp"

constexpr oid yadroTachSensorsTable_oid[] = {1, 3, 6, 1, 4, 1, 49769, 1, 4};
constexpr oid yadroTempSensorsTable_oid[] = {1, 3, 6, 1, 4, 1, 49769, 1, 2};
constexpr oid yadroVoltSensorsTable_oid[] = {1, 3, 6, 1, 4, 1, 49769, 1, 3};
constexpr oid yadroPowerSensorsTable_oid[] = {1, 3, 6, 1, 4, 1, 49769, 1, 6};
constexpr oid yadroCurrSensorsTable_oid[] = {1, 3, 6, 1, 4, 1, 49769, 1, 5};
/**
 * @brief Iterator hook routines
 *
 * generated by mib2c
 */
netsnmp_variable_list* yadroSensorsTable_get_next_data_point(
    void** my_loop_context,
    void** my_data_context,
    netsnmp_variable_list* put_index_data,
    netsnmp_iterator_info* mydata)
{
    size_t index       = reinterpret_cast<size_t>(*my_loop_context);
    sensors_arr_t& arr = *reinterpret_cast<sensors_arr_t*>(mydata->myvoid);

    while (index < arr.size() && !arr[index].isEnabled())
    {
        ++index;
    }

    if (index < arr.size() && arr[index].isEnabled())
    {
        sensor_t* entry = &arr[index];

        snmp_set_var_value(put_index_data, entry->name.c_str(), entry->name.length());

        *my_data_context = entry;
        *my_loop_context = reinterpret_cast<void*>(index + 1);
        return put_index_data;
    }

    return nullptr;
}
netsnmp_variable_list* yadroSensorsTable_get_first_data_point(
    void** my_loop_context,
    void** my_data_context,
    netsnmp_variable_list* put_index_data,
    netsnmp_iterator_info* mydata)
{
    *my_loop_context = reinterpret_cast<void*>(0);
    return yadroSensorsTable_get_next_data_point(my_loop_context,
            my_data_context,
            put_index_data,
            mydata);
}

/**
 * @brief handles requests for the yadroSensorsTable
 *
 * generated by mib2c
 */
int yadroSensorsTable_handler(netsnmp_mib_handler*           /*handler*/,
                              netsnmp_handler_registration* /*reginfo*/,
                              netsnmp_agent_request_info*   reqinfo,
                              netsnmp_request_info*         requests)
{
    DEBUGMSGTL(("yadro:handler", "Processing request (%d)\n", reqinfo->mode));

    switch (reqinfo->mode)
    {
        /*
         * Read-support (also covers GetNext requests)
         */
        case MODE_GET:
            for (netsnmp_request_info* request = requests; request; request = request->next)
            {
                auto entry = reinterpret_cast<sensor_t*>(netsnmp_extract_iterator_context(
                                 request));
                netsnmp_table_request_info* table_info = netsnmp_extract_table_info(request);

                switch (table_info->colnum)
                {
                    case COLUMN_YADROSENSOR_VALUE:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->currentValue);
                        break;

                    case COLUMN_YADROSENSOR_WARNLOW:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->warningLow);
                        break;

                    case COLUMN_YADROSENSOR_WARNHIGH:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->warningHigh);
                        break;

                    case COLUMN_YADROSENSOR_CRITLOW:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->criticalLow);
                        break;

                    case COLUMN_YADROSENSOR_CRITHIGH:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->criticalHigh);
                        break;

                    case COLUMN_YADROSENSOR_STATE:
                        if (!entry)
                        {
                            netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
                            continue;
                        }
                        snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                                   entry->state);
                        break;

                    default:
                        netsnmp_set_request_error(reqinfo, request,
                                                  SNMP_NOSUCHOBJECT);
                        break;
                }
            }
            break;
    }
    return SNMP_ERR_NOERROR;
}
/**
 * @brief Initialize the yadroSensorsTable table by defining its contents
 *        and how it's structured
 *
 * @param name          - human readable table name
 * @param table_oid     - OID of table
 * @param table_oid_len - OID length
 * @param sensors       - Pointer to sensors container
 */
void init_yadroSensorsTable(const char* name, const oid* table_oid,
                            size_t table_oid_len, sensors_arr_t* sensors)
{
    DEBUGMSGTL(("yadro:init", "Initialize %s\n", name));

    netsnmp_handler_registration* reg = netsnmp_create_handler_registration(
                                            name, yadroSensorsTable_handler,
                                            table_oid, table_oid_len,
                                            HANDLER_CAN_RONLY);

    netsnmp_table_registration_info* table_info = SNMP_MALLOC_TYPEDEF(
                netsnmp_table_registration_info);
    netsnmp_table_helper_add_indexes(table_info,
                                     ASN_OCTET_STR, /* index: yadro*SensorName */
                                     0);
    table_info->min_column = COLUMN_YADROSENSOR_VALUE;
    table_info->max_column = COLUMN_YADROSENSOR_STATE;

    netsnmp_iterator_info* iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);
    iinfo->get_first_data_point  = yadroSensorsTable_get_first_data_point;
    iinfo->get_next_data_point   = yadroSensorsTable_get_next_data_point;
    iinfo->table_reginfo         = table_info;
    iinfo->myvoid                = sensors;

    netsnmp_register_table_iterator(reg, iinfo);

    DEBUGMSGTL(("yadro:init", "Done initializing yadroSensorsTable.\n"));
}
/**
 * @brief Initialize yadro sensors tables
 */
void init_yadroTables(void)
{
    init_yadroSensorsTable("yadroTachSensorsTable",
                           yadroTachSensorsTable_oid,
                           OID_LENGTH(yadroTachSensorsTable_oid),
                           &tachometerSensors);

    init_yadroSensorsTable("yadroTempSensorsTable",
                           yadroTempSensorsTable_oid,
                           OID_LENGTH(yadroTempSensorsTable_oid),
                           &temperatureSensors);

    init_yadroSensorsTable("yadroVoltSensorsTable",
                           yadroVoltSensorsTable_oid,
                           OID_LENGTH(yadroVoltSensorsTable_oid),
                           &voltageSensors);

    init_yadroSensorsTable("yadroCurrSensorsTable",
                           yadroCurrSensorsTable_oid,
                           OID_LENGTH(yadroCurrSensorsTable_oid),
                           &currentSensors);

    init_yadroSensorsTable("yadroPowerSensorsTable",
                           yadroPowerSensorsTable_oid,
                           OID_LENGTH(yadroPowerSensorsTable_oid),
                           &powerSensors);
}
/**
 * @brief Drop yadro sensors tables
 */
void drop_yadroTables(void)
{
    DEBUGMSGTL(("yadro:shutdown", "Drop yadroVoltSensorsTable\n"));
    unregister_mib((oid*)yadroVoltSensorsTable_oid,
                   OID_LENGTH(yadroVoltSensorsTable_oid));

    DEBUGMSGTL(("yadro:shutdown", "Drop yadroTempSensorsTable\n"));
    unregister_mib((oid*)yadroTempSensorsTable_oid,
                   OID_LENGTH(yadroTempSensorsTable_oid));

    DEBUGMSGTL(("yadro:shutdown", "Drop yadroTachSensorsTable\n"));
    unregister_mib((oid*)yadroTachSensorsTable_oid,
                   OID_LENGTH(yadroTachSensorsTable_oid));

    DEBUGMSGTL(("yadro:shutdown", "Drop yadroCurrSensorsTable\n"));
    unregister_mib((oid*)yadroCurrSensorsTable_oid,
                   OID_LENGTH(yadroCurrSensorsTable_oid));

    DEBUGMSGTL(("yadro:shutdown", "Drop yadroPowerSensorsTable\n"));
    unregister_mib((oid*)yadroPowerSensorsTable_oid,
                   OID_LENGTH(yadroPowerSensorsTable_oid));
}