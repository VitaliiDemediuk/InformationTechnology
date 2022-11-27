import grpc

import grpc_generated.rows_service_pb2_grpc as rows_service

from grpc_generated import row_id_pb2
from grpc_generated import rows_service_pb2
from grpc_generated import table_id_pb2
from grpc_generated import empty_pb2

import grpc_generated.target as target


def edit_cell(table_id: int, column_idx: int, row_id: int, cell_data):
    with grpc.insecure_channel(target.target) as channel:
        stub = rows_service.RowsServiceStub(channel)
        grpc_table_id = table_id_pb2.TableId(id=table_id)
        grpc_cell_id = rows_service_pb2.CellId(rowId=row_id, columnIdx=column_idx, tableId=grpc_table_id)
        request = rows_service_pb2.CellEditRequest(cellId=grpc_cell_id, cell=cell_data)
        stub.editCell(request)


class CreateNewRowResolver:

    def __call__(self, obj, info, table_id):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = rows_service.RowsServiceStub(channel)
                request = table_id_pb2.TableId(id=table_id)
                stub.create(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class DeleteRowResolver:

    def __call__(self, obj, info, table_id, row_id):
        try:
            with grpc.insecure_channel(target.target) as channel:
                stub = rows_service.RowsServiceStub(channel)
                grpc_table_id = table_id_pb2.TableId(id=table_id)
                request = row_id_pb2.RowId(tableId=grpc_table_id, id=row_id)
                stub.deleteRow(request)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class EditIntCellResolver:

    def __call__(self, obj, info, table_id, column_idx, row_id, data):
        try:
            cell_data = rows_service_pb2.Cell(integer=data)
            edit_cell(table_id, column_idx, row_id, cell_data)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class EditRealCellResolver:

    def __call__(self, obj, info, table_id, column_idx, row_id, data):
        try:
            cell_data = rows_service_pb2.Cell(real=data)
            edit_cell(table_id, column_idx, row_id, cell_data)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class EditCharCellResolver:

    def __call__(self, obj, info, table_id, column_idx, row_id, data):
        try:
            assert len(data) == 1
            cell_data = rows_service_pb2.Cell(text=data)
            edit_cell(table_id, column_idx, row_id, cell_data)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class EditStringCellResolver:

    def __call__(self, obj, info, table_id, column_idx, row_id, data):
        try:
            cell_data = rows_service_pb2.Cell(text=data)
            edit_cell(table_id, column_idx, row_id, cell_data)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }


class ClearCellResolver:

    def __call__(self, obj, info, table_id, column_idx, row_id):
        try:
            cell_data = rows_service_pb2.Cell(empty=empty_pb2.Empty())
            edit_cell(table_id, column_idx, row_id, cell_data)
            return {
                "success": True
            }
        except Exception as error:
            return {
                "success": False,
                "errors": [str(error)]
            }