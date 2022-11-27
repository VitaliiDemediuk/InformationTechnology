import grpc

import grpc_generated.get_all_tables_service_pb2_grpc as get_all_tables_service
import grpc_generated.get_table_name_service_pb2_grpc as get_table_name_service

from grpc_generated import empty_pb2
from grpc_generated import column_id_pb2
from grpc_generated import table_id_pb2

import grpc_generated.target as target


class GetTableListResolver:

    def __call__(self, obj, info):
        try:
            tables_list = self.__get_tables_list()
            return {
                "success": True,
                "tables_list": tables_list
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }

    @staticmethod
    def __get_table_name(table_id: int) -> str:
        with grpc.insecure_channel(target.target) as channel:
            stub = get_table_name_service.TableNameGetterStub(channel)
            request = table_id_pb2.TableId(id=table_id)
            reply = stub.get(request)
            return reply.name

    def __get_tables_list(self) -> list:
        tables_list = list()
        with grpc.insecure_channel(target.target) as channel:
            stub = get_all_tables_service.AllTablesIdGetterStub(channel)
            request = empty_pb2.Empty()
            reply = stub.get(request)
            for table_id in reply.tablesId:
                tables_list.append({"id": table_id.id, "name": self.__get_table_name(table_id.id)})
        return tables_list
