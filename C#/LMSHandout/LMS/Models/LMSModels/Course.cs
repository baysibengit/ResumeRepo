﻿using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Course
{
    public uint Id { get; set; }

    public short Number { get; set; }

    public string Name { get; set; } = null!;

    public string Dept { get; set; } = null!;

    public virtual ICollection<Class> Classes { get; set; } = new List<Class>();

    public virtual Department DeptNavigation { get; set; } = null!;
}