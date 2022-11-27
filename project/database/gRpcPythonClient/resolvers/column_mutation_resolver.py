import grpc

import grpc_generated.columns_service_pb2_grpc as columns_service
import grpc_generated.target as target
from grpc_generated import column_id_pb2
from grpc_generated import columns_service_pb2
from grpc_generated import table_id_pb2


def create_column(table_id: int, column_info):
    with grpc.insecure_channel(target.target) as channel:
        stub = columns_service.ColumnsServiceStub(channel)
        grpc_table_id = table_id_pb2.TableId(id=table_id)
        request = columns_service_pb2.CreateColumnRequest(tableId=grpc_table_id, info=column_info)
        stub.create(request)


class CreateDefaultColumnResolver:

    def __call__(self, obj, info, table_id, name, column_type):
        try:
            default_info = columns_service_pb2.DefaultColumnInfo(
                type=columns_service_pb2.ColumnDataType.Value(column_type), name=name)
            column_info = columns_service_pb2.ColumnInfo(defaultInfo=default_info)
            create_column(table_id, column_info)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class CreateIntIntervalColumnResolver:

    def __call__(self, obj, info, table_id, name, lower_limit, upper_limit):
        try:
            interval_info = columns_service_pb2.IntervalIntColumnInfo(
                type=columns_service_pb2.ColumnDataType.Value("INTERVAL_INTEGER"), name=name,
                lowerLimit=lower_limit, upperLimit=upper_limit)
            column_info = columns_service_pb2.ColumnInfo(intIntervalInfo=interval_info)
            create_column(table_id, column_info)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class RenameColumnResolver:

    def __call__(self, obj, info, table_id, column_idx, new_name):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = columns_service.ColumnsServiceStub(channel)
                grpc_table_id = table_id_pb2.TableId(id=table_id)
                grpc_column_id = column_id_pb2.ColumnId(tableId=grpc_table_id, idx=column_idx)
                grpc_column_info = self.__get_column_info(table_id, column_idx)
                if grpc_column_info.HasField("defaultInfo"):
                    grpc_column_info.defaultInfo.name = new_name
                else:
                    grpc_column_info.intIntervalInfo.name = new_name
                request = columns_service_pb2.ColumnEditRequest(columnid=grpc_column_id, info=grpc_column_info)
                stub.edit(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }

    @staticmethod
    def __get_column_info(table_id: int, column_idx: int):
        with grpc.insecure_channel(target.target) as channel:
            stub = columns_service.ColumnsServiceStub(channel)
            grpc_table_id = table_id_pb2.TableId(id=table_id)
            request = column_id_pb2.ColumnId(tableId=grpc_table_id, idx=column_idx)
            return stub.get(request)


class DeleteColumnResolver:

    def __call__(self, obj, info, table_id, column_idx):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = columns_service.ColumnsServiceStub(channel)
                grpc_table_id = table_id_pb2.TableId(id=table_id)
                request = column_id_pb2.ColumnId(tableId=grpc_table_id, idx=column_idx)
                stub.deleteColumn(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }
