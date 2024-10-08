﻿using System;
using System.Collections.Generic;

namespace LMS.Models.LMSModels;

public partial class Submission
{
    public uint Score { get; set; }

    public DateTime Time { get; set; }

    public string? Contents { get; set; }

    public string Student { get; set; } = null!;

    public uint Assignment { get; set; }

    public virtual Assignment AssignmentNavigation { get; set; } = null!;

    public virtual Student StudentNavigation { get; set; } = null!;
}
