import grpc

import grpc_generated.create_table_service_pb2_grpc as create_table_service
import grpc_generated.rename_table_service_pb2_grpc as rename_table_service
import grpc_generated.delete_table_service_pb2_grpc as delete_table_service

from grpc_generated import table_id_pb2
from grpc_generated import table_name_pb2
from grpc_generated import rename_table_service_pb2

import grpc_generated.target as target


class CreateNewTableResolver:

    def __call__(self, obj, info, name):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = create_table_service.TableCreatorStub(channel)
                request = table_name_pb2.TableName(name=name)
                stub.create(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class RenameTableResolver:

    def __call__(self, obj, info, table_id, new_name):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = rename_table_service.RenameTableServiceStub(channel)
                grpc_table_id = table_id_pb2.TableId(id=table_id)
                request = rename_table_service_pb2.RenameTableRequest(name=new_name, tableId=grpc_table_id)
                stub.get(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class DeleteTableResolver:

    def __call__(self, obj, info, table_id):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = delete_table_service.TableDeleterStub(channel)
                request = table_id_pb2.TableId(id=table_id)
                stub.deleteTable(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }
