import grpc

import grpc_generated.get_all_rows_service_pb2_grpc as get_all_rows_service
import grpc_generated.rows_service_pb2_grpc as rows_service
import grpc_generated.columns_service_pb2_grpc as columns_service
import grpc_generated.get_columns_count_service_pb2_grpc as get_columns_count_service

from grpc_generated import table_id_pb2
from grpc_generated import column_id_pb2
from grpc_generated import row_id_pb2
from grpc_generated import columns_service_pb2

import grpc_generated.target as target


class GetTableResolver:

    def __call__(self, obj, info, table_id):
        try:
            table = self.__get_table(int(table_id))
            return {
                "success": True,
                "table": table
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }

    def __get_table(self, table_id: int):
        rows = list()
        column_type_list = self.__get_column_type_list(table_id)
        with grpc.insecure_channel(target.target) as channel:
            stub = get_all_rows_service.AllRowsGetterStub(channel)
            request = table_id_pb2.TableId(id=table_id)
            reply = stub.get(request)
            for row_id in reply.rowId:
                rows.append({
                    "row_id": row_id,
                    "cells": self.__get_cells(table_id, row_id, column_type_list)
                })
        return {"rows": rows}

    def __get_cells(self, table_id: int, row_id: int, column_type_list: list):
        cells = list()
        with grpc.insecure_channel(target.target) as channel:
            stub = rows_service.RowsServiceStub(channel)
            grpc_table_id = table_id_pb2.TableId(id=table_id)
            request = row_id_pb2.RowId(tableId=grpc_table_id, id=row_id)
            reply = stub.get(request)
            assert len(column_type_list) == len(reply.cells)
            for i in range(0, len(reply.cells)):
                grpc_cell = reply.cells[i]
                if grpc_cell.HasField("empty"):
                    cells.append(None)
                else:
                    match column_type_list[i]:
                        case "INTEGER":
                            cells.append({
                                "__typename": "IntegerWrapper",
                                "int_value": grpc_cell.integer
                            })
                        case "REAL":
                            cells.append({
                                "__typename": "RealWrapper",
                                "real_value": grpc_cell.real
                            })
                        case "CHAR":
                            cells.append({
                                "__typename": "CharWrapper",
                                "char_value": grpc_cell.text
                            })
                        case "STRING":
                            cells.append({
                                "__typename": "StringWrapper",
                                "str_value": grpc_cell.text
                            })
                        case "TEXT_FILE":
                            cells.append({
                                "__typename": "StringWrapper",
                                "filename": grpc_cell.file.name
                            })
                        case "INTERVAL_INTEGER":
                            cells.append({
                                "__typename": "IntervalInteger",
                                "int_value": grpc_cell.integer
                            })
        return cells

    def __get_column_type_list(self, table_id: int):
        column_type_list = list()
        with grpc.insecure_channel(target.target) as channel:
            stub = get_columns_count_service.ColumnsCountGetterStub(channel)
            request = table_id_pb2.TableId(id=table_id)
            reply = stub.get(request)
            for idx in range(0, reply.count):
                column_type_list.append(self.__get_column_type(table_id, idx))
        return column_type_list

    @staticmethod
    def __get_column_type(table_id: int, column_idx: int):
        with grpc.insecure_channel(target.target) as channel:
            stub = columns_service.ColumnsServiceStub(channel)
            grpc_table_id = table_id_pb2.TableId(id=table_id)
            request = column_id_pb2.ColumnId(tableId=grpc_table_id, idx=column_idx)
            reply = stub.get(request)
            if reply.HasField("intIntervalInfo"):
                return columns_service_pb2.ColumnDataType.Name(reply.intIntervalInfo.type)
            return columns_service_pb2.ColumnDataType.Name(reply.defaultInfo.type)

