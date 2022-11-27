import grpc

import grpc_generated.create_cartesian_product_service_pb2_grpc as create_cartesian_product_service

from grpc_generated import table_id_pb2
from grpc_generated import create_cartesian_product_service_pb2

import grpc_generated.target as target


class CreateCartesianProductResolver:

    def __call__(self, obj, info, first_table_id, second_table_id):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = create_cartesian_product_service.CartesianProductCreatorStub(channel)
                grpc_first_table_id = table_id_pb2.TableId(id=first_table_id)
                grpc_second_table_id = table_id_pb2.TableId(id=second_table_id)
                request = create_cartesian_product_service_pb2.CartesianProductReguest(firstId=grpc_first_table_id,
                                                                                       secondId=grpc_second_table_id)
                stub.create(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }