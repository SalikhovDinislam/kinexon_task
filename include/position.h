#ifndef GUARD_1DD5E526_FB39_43C5_A55A_64DB1A586C39
#define GUARD_1DD5E526_FB39_43C5_A55A_64DB1A586C39

#include "position.pb.h"

namespace position
{

void init(Data3d *position);
void move(Data3d *position, float time);
void apply_noize(Data3d *position);

}

#endif
