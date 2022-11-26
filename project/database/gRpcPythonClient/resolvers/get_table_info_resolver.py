import grpc

import grpc_generated.get_table_name_service_pb2_grpc as get_table_name_service
import grpc_generated.columns_service_pb2_grpc as columns_service
import grpc_generated.get_columns_count_service_pb2_grpc as get_columns_count_service

from grpc_generated import table_id_pb2
from grpc_generated import column_id_pb2
from grpc_generated import columns_service_pb2

import grpc_generated.target as target


class GetTableInfoResolver:

    def __call__(self, obj, info, table_id):
        try:
            table_info = self.__get_table_info(int(table_id))
            return {
                "success": True,
                "table_info": table_info
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }

    def __get_table_info(self, table_id: int):
        return {
            "table_name": self.__get_table_name(table_id),
            "columns": self.__get_columns(table_id)
        }

    @staticmethod
    def __get_table_name(table_id: int) -> str:
        with grpc.insecure_channel(target.target) as channel:
            stub = get_table_name_service.TableNameGetterStub(channel)
            request = table_id_pb2.TableId(id=table_id)
            reply = stub.get(request)
            return reply.name

    def __get_columns(self, table_id: int):
        columns = list()
        with grpc.insecure_channel(target.target) as channel:
            stub = get_columns_count_service.ColumnsCountGetterStub(channel)
            request = table_id_pb2.TableId(id=table_id)
            reply = stub.get(request)
            for idx in range(0, reply.count):
                columns.append(self.__get_column(table_id, idx))
        return columns

    @staticmethod
    def __get_column(table_id: int, column_idx: int):
        with grpc.insecure_channel(target.target) as channel:
            stub = columns_service.ColumnsServiceStub(channel)
            grpc_table_id = table_id_pb2.TableId(id=table_id)
            request = column_id_pb2.ColumnId(tableId=grpc_table_id, idx=column_idx)
            reply = stub.get(request)
            if reply.HasField("defaultInfo"):
                info = reply.defaultInfo
                return {
                    "name": info.name,
                    "type": columns_service_pb2.ColumnDataType.Name(info.type),
                }
            else:
                info = reply.intIntervalInfo
                return {
                    "name": info.name,
                    "type": columns_service_pb2.ColumnDataType.Name(info.type),
                    "lower_limit": info.lowerLimit,
                    "upper_limit": info.upperLimit
                }
