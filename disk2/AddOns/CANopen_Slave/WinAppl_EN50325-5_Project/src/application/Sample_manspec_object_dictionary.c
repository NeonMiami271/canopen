#include <DS401_header.h>

#if CHECK_VERSION_APPL(2, 1, 1)
//  CHECK_VERSION_APPL(2, 0, 0)
//  CHECK_VERSION_APPL(1, 2, 0)


#if (IOREMOTE_APPLICATION_MODE == APPLICATION_TEST)
#if (IOREMOTE_PRINTF_MODE == ON)
void print_appl(void *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stdout, (char *)fmt, ap);
	va_end(ap);
}
#else
void print_appl(void *fmt, ...)
{
}
#endif
#endif


union
{
  char buf [8];
  struct {
    char count;
    short int  d1;
    short int  d2;
    short int  d3;
    char reserv;
  } data;
} mess_214;

// ������� ����������� CANopen ��������.
// ��������� ���������� ��������� ������� CANopen �� ������ ������������ ���������� ������.
static void get_object_standev(struct object *obj)
{
	obj->size = CAN_ERRET_OBD_NOSUBIND;
	obj->deftype = 0;
	obj->access = 0;
	obj->pnt = NULL;

	// �������� CANopen �������� - ������ ������� ��� �����������.
	// � ���� ������ ���� �������� ������� �������� �� �������� ����������.
	
	if (obj->index == 0x2100) {		// �������� ������� �������, ��������� �������������.
		if (obj->subind == 0) {
			obj->size = sizeof(unsigned8);
			obj->deftype = CAN_DEFTYPE_UNSIGNED8;
			obj->access = CAN_MASK_ACCESS_RO | CAN_MASK_ACCESS_PDO;		// ������ ������ �� ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&mess_214.data.count;	// ������ ����������
		}
		return;
	}

	if (obj->index == 0x2101) {		// �������� ������� �������, ��������� �������������.
		if (obj->subind == 0) {
			obj->size = sizeof(int16);
			obj->deftype = CAN_DEFTYPE_INTEGER16;
			obj->access = CAN_MASK_ACCESS_RW | CAN_MASK_ACCESS_PDO;		// ������ �� ������, ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&mess_214.data.d1;	// ������ ����������
		}
		return;
	}

	if (obj->index == 0x2102) {		// �������� ������� �������, ��������� �������������.
		if (obj->subind == 0) {
			obj->size = sizeof(int16);
			obj->deftype = CAN_DEFTYPE_INTEGER16;
			obj->access = CAN_MASK_ACCESS_RW | CAN_MASK_ACCESS_PDO;		// ������ �� ������, ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&mess_214.data.d2;	// ������ ����������
		}
		return;
	}

	if (obj->index == 0x2103) {		// �������� ������� �������, ��������� �������������.
		if (obj->subind == 0) {
			obj->size = sizeof(int16);
			obj->deftype = CAN_DEFTYPE_INTEGER16;
			obj->access = CAN_MASK_ACCESS_RW | CAN_MASK_ACCESS_PDO;		// ������ �� ������, ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&mess_214.data.d3;	// ������ ����������
		}
		return;
	}

	if (obj->index == 0x2104) {		// �������� ������� �������, ��������� �������������.
		if (obj->subind == 0) {
			obj->size = sizeof(unsigned8);
			obj->deftype = CAN_DEFTYPE_UNSIGNED8;
			obj->access = CAN_MASK_ACCESS_RO | CAN_MASK_ACCESS_PDO;		// ������ ������ �� ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&mess_214.data.reserv;	// ������ ����������
		}
		return;
	}
	obj->size = CAN_ERRET_OBD_NOOBJECT;
}

// ������� ������� ������� CANopen �������
// ����������� ������� � ��������� �������� 0x2000..0x5FFF
int32 server_get_manspec_objsize(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	return obj.size;
}

// ������� ������� ���� ������� � CANopen �������
// ����������� ������� � ��������� �������� 0x2000..0x5FFF
int16 server_see_manspec_access(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	return obj.access;
}

// ������� ������� ���� CANopen �������
// ����������� ������� � ��������� �������� 0x2000..0x5FFF
int16 server_get_manspec_objtype(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	return obj.deftype;
}

// ������� ����������� ������ CANopen ������� �� ���������� �������
// ����������� ������� � ��������� �������� 0x2000..0x5FFF
int16 server_read_manspec_objdict(canindex index, cansubind subind, canbyte *data)
{
	int16 size, cnt;
	canbyte *bpnt;
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	if ((obj.access & CAN_MASK_ACCESS_RO) == 0) return CAN_ERRET_OBD_WRITEONLY;
	size = obj.size;
	bpnt = obj.pnt;
	OBJECT_DICTIONARY_TO_CANOPEN
	return CAN_RETOK;
}

// ������� ���������� ������ CANopen ������� � ��������� �������
// ����������� ������� � ��������� �������� 0x2000..0x5FFF
int16 server_write_manspec_objdict(canindex index, cansubind subind, canbyte *data)
{
	int16 size, cnt;
	canbyte *bpnt;
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	if ((obj.access & CAN_MASK_ACCESS_WO) == 0) return CAN_ERRET_OBD_READONLY;
	size = obj.size;
	bpnt = obj.pnt;
	FROM_CANOPEN_TO_OBJECT_DICTIONARY
	return CAN_RETOK;
}

void slave_init_manspec_application(void)
{
	// ����� ����� (����)���������������� ���������� ������, ������������ � �������� CANopen ��������.
}

void slave_init_manspec_communication(void)
{
  
	// ��������� PDO ����������� ��� CANopen ��������
  
	server_write_obd_u32(0x1A02, 0, 0);				// ����������� �������� TPDO (PDO invalid)
	server_write_obd_u32(0x1A02, 1, 0x21000008);	// ����������� ������� 0x2100 ������ 8 ��� � ������ TPDO
	server_write_obd_u32(0x1A02, 2, 0x21010010);	// ����������� ������� 0x2101 ������ 16 ��� � ������ TPDO
	server_write_obd_u32(0x1A02, 3, 0x21020010);	// ����������� ������� 0x2102 ������ 16 ��� � ������ TPDO
	server_write_obd_u32(0x1A02, 4, 0x21030010);	// ����������� ������� 0x2103 ������ 16 ��� � ������ TPDO
	server_write_obd_u32(0x1A02, 5, 0x21040008);	// ����������� ������� 0x2104 ������ 8 ��� � ������ TPDO
	server_write_obd_u32(0x1A02, 0, 5);				// ������ ����� ��������, ������������ � ������ TPDO (����)
													// ������ ����� ������ PDO �������� 64 ��� (8 ����)
	
	server_write_obd_u32(0x1802, 0x2, 2);			// �������� �������� PDO � ������� ������� SYNC - ��� �������������

	server_write_obd_u32(0x1802, 0x1, 0x80000214);	// ����-��������� COB-ID �������� TPDO �� �������� 0x214 � �� �������������� ��������� (PDO invalid)
													// �������������� ������ ��� ������������� ����-��������� ����������������� ������������� CANopen ���������������
													// ����� ����� ����� ���� ������������ �������������� �������� PDO COB-ID 0x00000214 (PDO valid)

	set_pdo_state(0x1802, VALID);					// ��������� �������� TPDO (PDO valid)
}

#endif
